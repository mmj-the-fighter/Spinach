# Constant GPU raytracing tile kernel for Ramanujan RJC (OpenCL _GPU_1)

def raytrace_tile_GPU_1(pixels, params, gid):
    t_left = params[0]
    t_top = params[1]
    t_width = params[2]
    
    # Calculate row (py) and col (px) within the tile
    py = gid / t_width
    FLOOR(py)
    px = gid - (py * t_width)
    
    i = t_left + px
    j = t_top + py
    
    pixel_dx = 0.005555555555555556
    pixel_dy = -0.005555555555555556
    pixel00_x = -1.775
    pixel00_y = 0.9972222222222222
    
    ray_x = pixel00_x + i * pixel_dx
    ray_y = pixel00_y + j * pixel_dy
    
    a = ray_x * ray_x + ray_y * ray_y + 1.0
    disc = 4.0 - 3.0 * a
    
    out_idx = gid * 3
    out_idx1 = out_idx + 1
    out_idx2 = out_idx + 2
    
    if disc >= 0.0:
        pixels[out_idx] = 255.0
        pixels[out_idx1] = 0.0
        pixels[out_idx2] = 0.0
    else:
        len_v = a
        SQRT(len_v)
        unit_y = ray_y / len_v
        t = 0.5 * (unit_y + 1.0)
        
        r_val = (1.0 - 0.5 * t) * 255.0
        g_val = (1.0 - 0.3 * t) * 255.0
        
        pixels[out_idx] = r_val
        pixels[out_idx1] = g_val
        pixels[out_idx2] = 255.0

# Allocate flat tile pixel buffer (160 * 180 * 3 = 86400)
pixels = [0 for _ in range(86400)]

# Load buffers into GPU memory
LOAD_MEM(pixels)
LOAD_MEM(params)

# Launch OpenCL GPU kernel with literal work-item count
raytrace_tile_GPU_1(pixels, params, 28800)

# Sync pixel buffer back to host for orchestrator dump
GPU_SYNC(pixels)
