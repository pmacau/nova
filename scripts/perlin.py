import numpy as np
import noise
import matplotlib.pyplot as plt

# Map size
width, height = 50, 50

# Noise parameters
scale = 20.0
octaves = 4
persistence = 2.0
lacunarity = 10.0

# Create a Perlin noise map
terrain = np.zeros((height, width))

for y in range(height):
    for x in range(width):
        nx, ny = x / width - 0.5, y / height - 0.5  # Center the noise
        terrain[y][x] = noise.pnoise2(nx * scale, ny * scale, octaves=octaves, persistence=persistence, lacunarity=lacunarity)

# Create a falloff map
falloff = np.zeros((height, width))
for y in range(height):
    for x in range(width):
        nx, ny = x / width * 2 - 1, y / height * 2 - 1  # Normalize to -1 to 1
        distance = np.sqrt(nx**2 + ny**2)  # Radial distance from center
        falloff[y][x] = distance**2  # Quadratic falloff

# Apply falloff to the terrain
island_map = terrain - falloff

# Classify terrain as land (1) or water (0)
threshold = -0.1
walkable_map = (island_map > threshold).astype(int)

# Display the map
plt.imshow(walkable_map, cmap='terrain')
plt.axis("off")
# plt.savefig("../data/maps/map.png", dpi=300, bbox_inches="tight")
plt.show()
walkable_map.astype(np.uint8).tofile("../data/maps/map.bin")
