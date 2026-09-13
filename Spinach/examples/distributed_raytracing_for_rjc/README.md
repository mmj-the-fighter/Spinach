# Distributed Raytracing for RJC (Spinach Example)

This example demonstrates real-time distributed raytracing using **Spinach** as the graphical frontend and **Ramanujan (RJC)** as the distributed compute backend.

The application decomposes an image into a grid of tiles (e.g., $160 \times 180$ pixels each), packages each tile's coordinates and viewport parameters into a CSV payload, and dispatches them asynchronously via HTTP REST to an RJC Homelab cluster. As cluster workers finish executing the raytracing kernels, the client downloads the computed RGB pixel buffers and renders them dynamically into an interactive SDL3 window.

---

## Features

- **Distributed Tiling**: Splits the viewport into parallel tiles to be computed across cluster nodes (desktop, server, phone).
- **Dual Compute Kernels**:
  - **CPU Kernel** (`raytrace_tile.py`): Executed on Ramanujan's CPU Rule Engine across worker threads.
  - **GPU Kernel** (`raytrace_tile_gpu.py`): Executed via OpenCL (`_GPU_1` device) on compatible worker nodes.
- **Disk-Based Kernel Loading**: Python kernels are loaded dynamically from disk files, making them easy to modify without recompiling C++.
- **Interactive Visualization**: Real-time rendering with tile borders, progress tracking, elapsed time, and ray throughput metrics.

---

## Directory Structure

```
distributed_raytracing_for_rjc/
├── README.md               # Instructions on building and running the app
├── CMakeLists.txt          # CMake target definition (in Spinach/CMakeLists.txt)
├── main.cpp                # Application entrypoint, SDL3 event/render loop, tile assembler
├── tiler.h                 # Tile grid generator and state tracker
├── rjc_client.h            # HTTP client for RJC API communication (/request, /fetch)
├── TileKernelGenerator.h   # Loads disk-based Python kernels and formats CSV parameters
├── raytrace_tile.py        # CPU raytracing kernel for RJC Rule Engine
└── raytrace_tile_gpu.py    # GPU OpenCL raytracing kernel for RJC
```

---

## 1. Prerequisites

1. **Ramanujan (RJC)**: Java 8+ runtime and the `rj` CLI / `developer-console` installed. To install `rj`, follow: https://github.com/Ramanujan-Computing/Ramanujan/blob/main/docs/build-and-usage.md.
2. **C++ Compiler**: Clang / GCC supporting C++17.
3. **CMake**: Version 3.16 or later.
4. **SDL3**: Automatically fetched and built by CMake if not installed system-wide.

---

## 2. Building the Application

From the `Spinach/Spinach` directory:

```bash
# Generate build files
cmake -B build

# Build the distributed raytracing executable
cmake --build build --target distributed_raytracing_for_rjc
```

The resulting binary will be placed at `Spinach/Spinach/build/distributed_raytracing_for_rjc`.

---

## 3. Starting the Ramanujan Cluster

Before running the raytracer, start the RJC Homelab orchestrator and at least one worker:

### Step 3.1: Start the Homelab Server
In Terminal 1:
```bash
# Using the rj CLI:
rj homelab 8888

# Or using java directly:
RAMANUJAN_WS=/path/to/ramanujan java -jar developer-console-1.0-SNAPSHOT-fat.jar homelab 8888
```

### Step 3.2: Start Cluster Worker(s)
In Terminal 2 (and optionally on other machines or mobile devices on the network):
```bash
# Using the rj CLI (connects 8 worker threads to the server):
rj worker http://127.0.0.1:8888 8

# Or using java directly:
RAMANUJAN_WS=/path/to/ramanujan java -jar developer-console-1.0-SNAPSHOT-fat.jar worker http://127.0.0.1:8888 8
```

---

## 4. Running the Application

### Running with GPU Acceleration
To dispatch tiles using the OpenCL GPU kernel (`raytrace_tile_gpu.py`):
```bash
./build/distributed_raytracing_for_rjc --gpu
```

### Running with CPU Rule Engine
To dispatch tiles using the CPU kernel (`raytrace_tile.py`):
```bash
./build/distributed_raytracing_for_rjc --cpu
```

### Specifying Custom Host and Port
If the Homelab server is hosted on a different machine or port:
```bash
# Syntax: ./build/distributed_raytracing_for_rjc [--gpu|--cpu] [port] [host]
./build/distributed_raytracing_for_rjc --gpu 8888 192.168.1.151
```

---

## 5. Kernel Customization

The raytracing kernels are standalone Python files loaded directly from disk at runtime:
- To customize camera angles, sphere positions, shading, or multi-sampling on CPU, edit [`raytrace_tile.py`](raytrace_tile.py).
- To edit the OpenCL GPU kernel, modify the OpenCL C string inside [`raytrace_tile_gpu.py`](raytrace_tile_gpu.py).

Changes take effect on the next application run without needing to recompile the C++ binary.
