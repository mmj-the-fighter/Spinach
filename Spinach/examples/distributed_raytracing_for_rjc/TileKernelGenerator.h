#ifndef TILE_KERNEL_GENERATOR_H
#define TILE_KERNEL_GENERATOR_H

#include "tiler.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <climits>
#include <stdexcept>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#define mkdir(dir, mode) _mkdir(dir)
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace rtutil
{
    // Resolve constant Python kernel file on disk (both CPU and GPU live in standalone .py files)
    inline std::string EnsureConstantKernelFile(bool useGpu = false, const std::string& preferredDir = "") {
        std::string filename = useGpu ? "raytrace_tile_gpu.py" : "raytrace_tile.py";
        std::vector<std::string> searchPaths = {
            preferredDir.empty() ? "" : preferredDir + "/" + filename,
            filename,
            "examples/distributed_raytracing_for_rjc/" + filename,
            "../examples/distributed_raytracing_for_rjc/" + filename,
            "../../examples/distributed_raytracing_for_rjc/" + filename,
            "Spinach/examples/distributed_raytracing_for_rjc/" + filename
        };

        for (const auto& path : searchPaths) {
            if (path.empty()) continue;
            std::ifstream f(path);
            if (f.good()) {
#ifdef _WIN32
                char resolved[MAX_PATH];
                if (GetFullPathNameA(path.c_str(), MAX_PATH, resolved, nullptr)) {
                    std::string resStr(resolved);
                    for (char& c : resStr) {
                        if (c == '\\') c = '/';
                    }
                    return resStr;
                }
#else
                char resolved[PATH_MAX];
                if (realpath(path.c_str(), resolved)) {
                    return std::string(resolved);
                }
#endif
                return path;
            }
        }

        std::cerr << "[SpinachRT] ERROR: Required kernel file '" << filename << "' not found on disk!\n";
        throw std::runtime_error("Kernel file not found on disk: " + filename);
    }

    inline std::string WriteTileParamsCsv(const std::string& tileDir, const Rect& tile) {
        mkdir(tileDir.c_str(), 0755);
        std::string paramsPath = tileDir + "/params.csv";
        std::ofstream f(paramsPath);
        f << static_cast<double>(tile.left) << ","
          << static_cast<double>(tile.top) << ","
          << static_cast<double>(tile.width) << ","
          << static_cast<double>(tile.height) << "\n";
        return paramsPath;
    }
}

#endif
