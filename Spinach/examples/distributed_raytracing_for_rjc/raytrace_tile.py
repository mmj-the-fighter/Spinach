# Constant raytracing tile kernel for Ramanujan RJC
t_left = params[0]
t_top = params[1]
t_width = params[2]
t_height = params[3]

# Tile pixel buffer (160 * 180 * 3 = 86400)
pixels = [0 for _ in range(86400)]

pixel_dx = 0.005555555555555556
pixel_dy = -0.005555555555555556
pixel00_x = -1.775
pixel00_y = 0.9972222222222222

py = 0.0
out_idx = 0
while py < t_height:
    j = t_top + py
    ray_y = pixel00_y + j * pixel_dy
    px = 0.0
    while px < t_width:
        i = t_left + px
        ray_x = pixel00_x + i * pixel_dx
        
        a = ray_x * ray_x + ray_y * ray_y + 1.0
        disc = 4.0 - 3.0 * a
        
        if disc >= 0.0:
            pixels[out_idx] = 255.0
            out_idx += 1
            pixels[out_idx] = 0.0
            out_idx += 1
            pixels[out_idx] = 0.0
            out_idx += 1
        else:
            len_v = a
            SQRT(len_v)
            unit_y = ray_y / len_v
            t = 0.5 * (unit_y + 1.0)
            
            r_val = (1.0 - 0.5 * t) * 255.0
            g_val = (1.0 - 0.3 * t) * 255.0
            
            pixels[out_idx] = r_val
            out_idx += 1
            pixels[out_idx] = g_val
            out_idx += 1
            pixels[out_idx] = 255.0
            out_idx += 1
            
        px += 1.0
    py += 1.0
