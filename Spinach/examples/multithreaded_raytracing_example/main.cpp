//Multi threaded raytracing example
//This file was developed with assistance of AI
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

#include <spn_canvas.h>
#include <spn_core.h>

#include "RayTracer.h"
#include <tiler.h>
#include <spn_profiler.h>
#include <spn_utils.h>

enum
{
    FREE,
    TAKEN
};

struct Task
{
    rsplitter::Rect rect;
    int status;
};

int program(int argc, char* argv[])
{
    PROFILE_REST_OF_THE_CURRENT_BLOCK(100)

    constexpr int imageWt = 640;
    constexpr double aspectRatio = 16.0 / 9.0;

    constexpr int imageHt =
        RayTracer::CalculateImageHeight(aspectRatio, imageWt);


    // Divide image into rectangular regions
    rsplitter::Rect rect;
    rect.left = 0;
    rect.top = 0;
    rect.width = imageWt;
    rect.height = imageHt;

    const int numOfThreads = 4;

    int optimumRows;
    int optimumCols;

    rsplitter::ComputeGrid(
        numOfThreads,
        imageWt,
        imageHt,
        optimumRows,
        optimumCols,
        2
    );

    std::cout
        << "Tiling "
        << imageWt << "X" << imageHt
        << " into "
        << optimumRows << " Rows and "
        << optimumCols << " Cols"
        << "\n"
        << "each having approximately "
        << imageWt / optimumCols
        << "X"
        << imageHt / optimumRows
        << " size\n";

    const std::vector<rsplitter::Rect> rvec =
        rsplitter::SplitRectangleToTiles(
            rect,
            optimumRows,
            optimumCols
        );

    std::cout
        << "Actual number of tiles: "
        << rvec.size()
        << std::endl;


    // Create task list
    std::vector<Task> taskvec;
    taskvec.reserve(rvec.size());

    for (const auto& r : rvec) {
        taskvec.push_back({ r, FREE });
    }

    // Number of tiles that have NOT finished rendering.
    std::atomic<size_t> remaining{ rvec.size() };

    // Index of next task to assign.
    std::atomic<size_t> nextTask{ 0 };

    // Task server
    auto getTileRenderingTask =
        [&taskvec, &nextTask]() -> rsplitter::Rect
        {
            const size_t i =
                nextTask.fetch_add(
                    1,
                    std::memory_order_relaxed
                );

            if (i < taskvec.size()) {
                return taskvec[i].rect;
            }

            // No more tasks.
            return rsplitter::Rect{ 0, 0, 0, 0 };
        };

    auto notifyTileFinished =
        [&remaining]()->void {
        const size_t previous =
            remaining.fetch_sub(
                1,
                std::memory_order_acq_rel
            );
        }
    ;


    // Initialize Spinach
    spn::SpinachCore sc;

    if (!sc.Init(imageWt, imageHt, "../res/"))
    {
        std::cout
            << "initialization failed with error "
            << sc.GetInitializationResult()
            << std::endl;

        return 1;
    }


    sc.SetWindowTitle(
        "RT in one weekend: First example"
    );

    sc.GetCanvas()->SetClearColorUint(0x0);
    sc.GetCanvas()->Clear();


    // Create RayTracers
    std::vector<std::unique_ptr<RayTracer>> rtvec;
    std::vector<std::thread> threadVec;

    rtvec.reserve(numOfThreads);
    threadVec.reserve(numOfThreads);

    for (int i = 0; i < numOfThreads; ++i) {
        auto prt =
            std::make_unique<RayTracer>(
                getTileRenderingTask, 
                notifyTileFinished);

        prt->Init(
            sc.GetCanvas(),
            1.0,
            2.0,
            point3(0, 0, 0)
        );

        rtvec.push_back(std::move(prt));

        RayTracer* rt = rtvec.back().get();

        threadVec.emplace_back(
            [rt, &sc, &remaining]()
            {
                rt->Render();
                
                if (remaining.load() == 0) {
                    sc.SetUserWantsToQuit(true);
                }
            }
        );
    }


    //detach workers
    for (auto& t : threadVec)
    {
        if (t.joinable()) {
            t.detach();
        }
    }
    
    sc.MainLoop();
    
    //save screenshot only if last tile was finished.
    if (remaining.load() == 0) {
        char fileName[256];
        spn::GetFilenameFromCurrentTime(fileName, "SpnRT", ".png");
        sc.SaveScreenShot(fileName);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int rv = program(argc, argv);

    PROFILE_PRINT_PROFILER_OUTPUT;

    return rv;
}