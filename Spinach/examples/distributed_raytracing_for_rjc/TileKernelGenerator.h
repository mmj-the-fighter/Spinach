#ifndef TILE_KERNEL_GENERATOR_H
#define TILE_KERNEL_GENERATOR_H

#include "tiler.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <climits>
#include <unistd.h>
#include <stdexcept>

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
            "Spinach/examples/distributed_raytracing_for_rjc/" + filename,
            "/Users/pranav/Desktop/ramanujan_oss/Spinach/Spinach/examples/distributed_raytracing_for_rjc/" + filename
        };

        for (const auto& path : searchPaths) {
            if (path.empty()) continue;
            std::ifstream f(path);
            if (f.good()) {
                char resolved[PATH_MAX];
                if (realpath(path.c_str(), resolved)) {
                    return std::string(resolved);
                }
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
