from PIL import Image
import numpy as np
import json

# Predefined colors
predefined_colors = [
    (49, 51, 56),
    (28, 30, 33),
    (43, 45, 49),
    (56, 58, 64),
    (86, 99, 242)
]

# Number of grayscale colors
num_predefined = len(predefined_colors)
num_grayscale = 8
num_rainbow = 3 * 3 * 27  # 3 saturation levels, 3 brightness groups, 27 hues

# Create an empty list to store the colors
colors = []

# Add predefined colors
colors.extend(predefined_colors)

# Generate grayscale colors
for i in range(num_grayscale):
    gray_value = int(255 * (i / (num_grayscale - 1)))
    colors.append((gray_value, gray_value, gray_value))

# Function to convert HSV to RGB
def hsv_to_rgb(hue, saturation, value):
    h = hue * 6
    i = int(h)
    f = h - i
    p = value * (1 - saturation)
    q = value * (1 - f * saturation)
    t = value * (1 - (1 - f) * saturation)
    value = value * 255
    p = int(p * 255)
    q = int(q * 255)
    t = int(t * 255)
    value = int(value)
    i = i % 6
    if i == 0:
        return (value, t, p)
    if i == 1:
        return (q, value, p)
    if i == 2:
        return (p, value, t)
    if i == 3:
        return (p, q, value)
    if i == 4:
        return (t, p, value)
    if i == 5:
        return (value, p, q)

# Generate rainbow colors with specific hue, saturation, and brightness levels
def generate_rainbow_colors(hues, brightness_levels, saturation_levels):
    rainbow_colors = []
    num_brightness = len(brightness_levels)
    for s in saturation_levels:
        for b in brightness_levels:
            for h in range(hues):
                hue = h / hues
                rgb = hsv_to_rgb(hue, s, b)
                rainbow_colors.append(rgb)
    return rainbow_colors

# Generate colors with 3 saturation levels, 3 brightness levels, and 27 hues
saturation_levels = [0.3, 0.6, 1.0]  # Three saturation levels
brightness_levels = [0.15, 0.575, 1]      # Three brightness levels
colors.extend(generate_rainbow_colors(27, brightness_levels, saturation_levels))

# Ensure we only keep the first 256 colors
colors = colors[:256]

# Create an image to display the palette
palette_width = 256
palette_height = 50
image = Image.new('RGB', (palette_width, palette_height))
pixels = np.array(image)

# Fill the image with colors from the palette
for i, color in enumerate(colors):
    pixels[:, i] = color

# Save the image
image = Image.fromarray(pixels)
image.save('palette.png')

# Save the color palette data
with open('palette.json', 'w') as f:
    json.dump(colors, f)

yaml_out = """palettes:
  - name: main_palette
    images: automatic
    fixed-entries:
"""

for i, color in enumerate(colors):
    yaml_out += "      - color: {index: "+str(i)+", "
    yaml_out += f"r: {color[0]}, g: {color[1]}, b: {color[2]}"+"}\n"

with open("palette.yaml", 'w') as f:
    f.write(yaml_out)

print("Palette image and JSON file have been saved.")
