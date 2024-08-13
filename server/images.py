from PIL import Image
import numpy as np
import pyopencl as cl
import json

with open("palette.json", 'r') as f:
    colors = json.load(f)
    colors = np.array(colors, dtype=np.float32)

platforms = cl.get_platforms()
device = platforms[0].get_devices(cl.device_type.GPU)[0]  # Get the first GPU device
context = cl.Context([device])
queue = cl.CommandQueue(context)

kernel_code = """
__kernel void nearest_color(__global const float *image_data, 
                            __global const float *palette, 
                            __global uchar *output_data, 
                            const unsigned int img_width, 
                            const unsigned int img_height, 
                            const unsigned int palette_size) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int idx = (y * img_width + x) * 3;
    
    float3 color = (float3)(image_data[idx], image_data[idx+1], image_data[idx+2]);
    
    float min_dist = FLT_MAX;
    int nearest_idx = 0;

    for (int i = 0; i < palette_size; ++i) {
        float3 palette_color = (float3)(palette[i*3], palette[i*3+1], palette[i*3+2]);
        float3 diff = color - palette_color;
        float dist = dot(diff, diff);
        if (dist < min_dist) {
            min_dist = dist;
            nearest_idx = i;
        }
    }

    output_data[y * img_width + x] = (uchar)nearest_idx;
}
"""

program = cl.Program(context, kernel_code).build()

def encode_image(image: Image.Image, resolution: tuple) -> Image.Image:
    image = image.resize(resolution)
    
    data = np.array(image, dtype=np.float32)
    height, width, _ = data.shape
    
    image_buffer = cl.Buffer(context, cl.mem_flags.READ_ONLY | cl.mem_flags.COPY_HOST_PTR, hostbuf=data)
    palette_buffer = cl.Buffer(context, cl.mem_flags.READ_ONLY | cl.mem_flags.COPY_HOST_PTR, hostbuf=colors)
    output_buffer = cl.Buffer(context, cl.mem_flags.WRITE_ONLY, width * height * np.uint8().itemsize)
    
    global_work_size = (width, height)
    program.nearest_color(queue, global_work_size, None, image_buffer, palette_buffer, output_buffer, np.uint32(width), np.uint32(height), np.uint32(len(colors)))
    
    output_data = np.empty(width * height, dtype=np.uint8)  # Flattened output array
    cl.enqueue_copy(queue, output_data, output_buffer).wait()
    
    return output_data
