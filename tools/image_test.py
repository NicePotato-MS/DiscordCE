from PIL import Image
import json
import numpy as np
import pyopencl as cl

# Load the color palette
with open('palette.json', 'r') as f:
    colors = json.load(f)

# Convert the palette to a NumPy array for processing
colors = np.array(colors, dtype=np.float32)

# OpenCL setup
platforms = cl.get_platforms()
device = platforms[0].get_devices(cl.device_type.GPU)[0]  # Get the first GPU device
context = cl.Context([device])
queue = cl.CommandQueue(context)

# Define OpenCL kernel code
kernel_code = """
__kernel void nearest_color(__global const float *image_data, 
                            __global const float *palette, 
                            __global float *output_data, 
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

    output_data[idx] = palette[nearest_idx*3];
    output_data[idx+1] = palette[nearest_idx*3+1];
    output_data[idx+2] = palette[nearest_idx*3+2];
}
"""

# Compile the OpenCL kernel
program = cl.Program(context, kernel_code).build()

# Load the input image
input_image_path = 'input_image.png'  # Update this to your input image path
image = Image.open(input_image_path)
image = image.convert('RGB')

# Resize the image to 24x24
image = image.resize((1024, 1024))

# Prepare image data for OpenCL
data = np.array(image, dtype=np.float32)
height, width, _ = data.shape

# Create OpenCL buffers
image_buffer = cl.Buffer(context, cl.mem_flags.READ_ONLY | cl.mem_flags.COPY_HOST_PTR, hostbuf=data)
palette_buffer = cl.Buffer(context, cl.mem_flags.READ_ONLY | cl.mem_flags.COPY_HOST_PTR, hostbuf=colors)
output_buffer = cl.Buffer(context, cl.mem_flags.WRITE_ONLY, data.nbytes)

# Execute the kernel
global_work_size = (width, height)
program.nearest_color(queue, global_work_size, None, image_buffer, palette_buffer, output_buffer, np.uint32(width), np.uint32(height), np.uint32(len(colors)))

# Retrieve the result
output_data = np.empty_like(data)
cl.enqueue_copy(queue, output_data, output_buffer).wait()

# Save the output image
output_image = Image.fromarray(np.uint8(output_data))
output_image.save('output_image.png')
