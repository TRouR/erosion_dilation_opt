import numpy as np
import matplotlib.pyplot as plt

# File path and image dimensions
file_path = "out_cherry_200x200_binary.yuv"
width, height = 200, 200

# Read raw binary image data
with open(file_path, "rb") as f:
    data = np.frombuffer(f.read(), dtype=np.uint8)

# Reshape into 2D image
image = data.reshape((height, width))

# Display
plt.imshow(image, cmap='gray')
plt.axis('off')
plt.show()
