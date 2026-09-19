#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <sstream>
#include <fstream>
#include <future>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <direct.h>
#define rmdir(d) _rmdir(d)
#else
#include <unistd.h>
#endif

#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_profiler.h>
#include <spn_utils.h>

#include "tiler.h"
#include "TileKernelGenerator.h"
#include "rjc_client.h"

struct CompletedTile
{
    rtutil::Rect rect;
    std::vector<uint8_t> rgb;
};

// Parse a flat comma-separated line of floats into RGB bytes
static std::vector<uint8_t> ParseTileCsv(const std::string& csvPath, int expectedCount)
{
    std::vector<uint8_t> result;
    result.reserve(expectedCount);

    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "[SpinachRT] Failed to open tile CSV: " << csvPath << "\n";
        result.resize(expectedCount, 0);
        return result;
    }

    std::string token;
    while (std::getline(file, token, ',')) {
        if (token.empty() || token == "\n") continue;
        try {
            float val = std::stof(token);
            if (val < 0.0f) val = 0.0f;
            if (val > 255.0f) val = 255.0f;
            result.push_back(static_cast<uint8_t>(val));
        } catch (...) {}
    }

    if (result.size() < static_cast<size_t>(expectedCount)) {
        result.resize(expectedCount, 0);
    }
    return result;
}

int program(int argc, char* argv[])
{
    PROFILE_REST_OF_THE_CURRENT_BLOCK(100)

    std::string rjcHost = "127.0.0.1";
    int rjcPort = 8888;
    bool useGpu = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--gpu" || arg == "-gpu" || arg == "gpu") {
            useGpu = true;
        } else if (arg == "--cpu" || arg == "-cpu" || arg == "cpu") {
            useGpu = false;
        } else if (arg.find('.') != std::string::npos && !std::isdigit(arg[0])) {
            rjcHost = arg;
        } else {
            try {
                int p = std::stoi(arg);
                if (p > 0 && p <= 65535) {
                    rjcPort = p;
                } else {
                    rjcHost = arg;
                }
            } catch (...) {
                rjcHost = arg;
            }
        }
    }

    std::cout << "[SpinachRT] RJC Orchestrator target: " << rjcHost << ":" << rjcPort << "\n";
    std::cout << "[SpinachRT] Compute Mode: " << (useGpu ? "GPU (OpenCL _GPU_1)" : "CPU") << "\n";

    constexpr int imageWt = 640;
    constexpr double aspectRatio = 16.0 / 9.0;
    constexpr int imageHt = static_cast<int>(imageWt / aspectRatio); // 360

    // ------------------------------------------------------------
    // 1. Divide image into rectangular regions using tiler.h
    // ------------------------------------------------------------
    rtutil::Rect rect = rtutil::MakeRect(0, 0, imageWt, imageHt);

    const int numOfWorkers = 4;
    int optimumRows = 0;
    int optimumCols = 0;

    rtutil::ComputeGrid(
        numOfWorkers,
        imageWt,
        imageHt,
        optimumRows,
        optimumCols,
        2
    );

    std::cout
        << "[SpinachRT] Tiling " << imageWt << "x" << imageHt
        << " into " << optimumRows << " Rows and " << optimumCols << " Cols"
        << " (tile size ~ " << imageWt / optimumCols << "x" << imageHt / optimumRows << ")\n";

    const std::vector<rtutil::Rect> rvec =
        rtutil::SplitRectangleToTiles(rect, optimumRows, optimumCols);

    const size_t totalTiles = rvec.size();
    std::cout << "[SpinachRT] Total tile count: " << totalTiles << std::endl;

    // ------------------------------------------------------------
    // 2. Initialize Spinach Core and Canvas
    // ------------------------------------------------------------
    spn::SpinachCore sc;

    if (!sc.Init(imageWt, imageHt, "../res/"))
    {
        std::cerr << "[SpinachRT] Initialization failed with error: "
                  << sc.GetInitializationResult() << std::endl;
        return 1;
    }

    std::string winTitle = std::string("Distributed Raytracing for RJC (") + (useGpu ? "GPU" : "CPU") + ")";
    sc.SetWindowTitle(winTitle.c_str());
    sc.GetCanvas()->SetClearColorUint(0x00000000);
    sc.GetCanvas()->Clear();

    // ------------------------------------------------------------
    // 3. Tile completion tracking and thread-safe queue
    // ------------------------------------------------------------
    std::mutex queueMutex;
    std::vector<CompletedTile> completedQueue;
    std::atomic<size_t> tilesReceived{ 0 };

    auto onTileCompleted = [&](const rtutil::Rect& tileRect, std::vector<uint8_t> rgbPixels, int tileIndex) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            completedQueue.push_back({ tileRect, std::move(rgbPixels) });
        }
        size_t done = tilesReceived.fetch_add(1) + 1;
        std::cout << "[SpinachRT] Tile " << tileIndex << " received ("
                  << done << "/" << totalTiles << " completed)\n";
    };

    // Ensure constant kernel file exists and obtain its path
    std::string kernelPath = rtutil::EnsureConstantKernelFile(useGpu);
    std::cout << "[SpinachRT] Using constant kernel: " << kernelPath << "\n";

    // ------------------------------------------------------------
    // 4. Dispatch N orchestrator calls asynchronously for each tile
    // ------------------------------------------------------------
    std::vector<std::future<void>> asyncDispatches;
    asyncDispatches.reserve(totalTiles);

    auto sessionTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    for (size_t i = 0; i < totalTiles; ++i)
    {
        rtutil::Rect tile = rvec[i];
        int tileIdx = static_cast<int>(i);

        asyncDispatches.push_back(std::async(std::launch::async, [=, &onTileCompleted]() {
            std::ostringstream tileDirStream;
#ifdef _WIN32
            tileDirStream << "C:/tmp/rt_tile_" << sessionTimestamp << "_" << tileIdx;
#else
            tileDirStream << "/tmp/rt_tile_" << sessionTimestamp << "_" << tileIdx;
#endif
            std::string tileDir = tileDirStream.str();

            std::string paramsCsvPath = rtutil::WriteTileParamsCsv(tileDir, tile);
            std::string csvPath = tileDir + "/pixels.csv";
            int expectedBytes = tile.width * tile.height * 3;

            bool success = false;
            constexpr int maxRunRetries = 3;
            constexpr int maxDumpRetries = 5;

            for (int runAttempt = 0; runAttempt < maxRunRetries && !success; ++runAttempt) {
                std::string reqId = "rt_tile_" + std::to_string(sessionTimestamp) + "_"
                    + std::to_string(tileIdx) + (runAttempt > 0 ? ("_r" + std::to_string(runAttempt)) : "");

                // POST to /orchestrator/run with constant kernel and tile params.csv
                std::ostringstream runPayload;
                runPayload << "{\"args\":[\"" << kernelPath << "\",\"" << paramsCsvPath << "\"],\"requestId\":\"" << reqId << "\"}";

                rjc::HttpResponse runResp = rjc::HttpClient::PostJson(
                    rjcHost, rjcPort, "/orchestrator/run", runPayload.str(), 180);

                if (runResp.statusCode != 200) {
                    std::cerr << "[SpinachRT] Error on /orchestrator/run for tile " << tileIdx
                              << " (attempt " << runAttempt + 1 << ", status " << runResp.statusCode << "): " << runResp.body << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    continue;
                }

                // Poll /orchestrator/dump with retries
                std::ostringstream dumpPayload;
                dumpPayload << "{\"requestId\":\"" << reqId << "\",\"name\":\"pixels\",\"path\":\"" << csvPath << "\"}";
                std::string dumpPayloadStr = dumpPayload.str();

                for (int dumpAttempt = 0; dumpAttempt < maxDumpRetries; ++dumpAttempt) {
                    rjc::HttpResponse dumpResp = rjc::HttpClient::PostJson(
                        rjcHost, rjcPort, "/orchestrator/dump", dumpPayloadStr, 60);

                    if (dumpResp.statusCode == 200) {
                        success = true;
                        break;
                    }

                    // If 404, processing may still be finalizing or registering; wait and retry
                    std::cerr << "[SpinachRT] /orchestrator/dump for tile " << tileIdx
                              << " returned status " << dumpResp.statusCode << " (dump attempt "
                              << dumpAttempt + 1 << "/" << maxDumpRetries << "): " << dumpResp.body << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(100 * (dumpAttempt + 1)));
                }

                if (!success) {
                    std::cerr << "[SpinachRT] Retrying full run for tile " << tileIdx
                              << " (run attempt " << runAttempt + 1 << "/" << maxRunRetries << " failed)\n";
                }
            }

            std::vector<uint8_t> rgb;
            if (success) {
                rgb = ParseTileCsv(csvPath, expectedBytes);
            } else {
                std::cerr << "[SpinachRT] FAILED to retrieve tile " << tileIdx
                          << " after retries. Falling back to black pixels.\n";
                rgb.resize(expectedBytes, 0);
            }

            // Clean up temporary tile directory and files
            std::remove(paramsCsvPath.c_str());
            std::remove(csvPath.c_str());
            rmdir(tileDir.c_str());

            // Trigger tile callback
            onTileCompleted(tile, std::move(rgb), tileIdx);
        }));
    }

    // ------------------------------------------------------------
    // 5. Ingestion in Main Loop Handler (progressive painting)
    // ------------------------------------------------------------
    sc.SetUpdateAndRenderHandler([&](spn::Canvas* canvas) {
        std::vector<CompletedTile> batch;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!completedQueue.empty()) {
                batch.swap(completedQueue);
            }
        }

        for (const auto& item : batch) {
            size_t idx = 0;
            for (int y = item.rect.top; y < item.rect.top + item.rect.height; ++y) {
                for (int x = item.rect.left; x < item.rect.left + item.rect.width; ++x) {
                    if (idx + 2 < item.rgb.size()) {
                        uint8_t r = item.rgb[idx++];
                        uint8_t g = item.rgb[idx++];
                        uint8_t b = item.rgb[idx++];
                        canvas->SetPixel(x, y, r, g, b);
                    }
                }
            }
        }

        if (tilesReceived.load() >= totalTiles && batch.empty()) {
            std::cout << "[SpinachRT] All " << totalTiles << " tiles received. Finalizing frame...\n";
            sc.SetUserWantsToQuit(true);
        }
    });

    // ------------------------------------------------------------
    // 6. Run SDL3 Event & Rendering loop
    // ------------------------------------------------------------
    sc.MainLoop();

    // ------------------------------------------------------------
    // 7. Save Screenshot upon completion
    // ------------------------------------------------------------
    if (tilesReceived.load() >= totalTiles)
    {
        char fileName[256];
        spn::GetFilenameFromCurrentTime(fileName, "SpnRT_RJC", ".png");
        sc.SaveScreenShot(fileName);
        std::cout << "[SpinachRT] Screenshot saved: " << fileName << std::endl;
    }

    // Wait for any remaining async dispatches
    for (auto& f : asyncDispatches) {
        if (f.valid()) {
            f.wait();
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int rv = program(argc, argv);
    PROFILE_PRINT_PROFILER_OUTPUT;
    return rv;
}
