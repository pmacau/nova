import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from noise import pnoise2


def island_colormap():
    colors = [(0, 0, 0), (1, 1, 1), (0, 1, 1), (0, 0, 1)]
    cmap_name = "island_colormap"
    return LinearSegmentedColormap.from_list(cmap_name, colors)


def perlin_map(
        width, height,
        scale=50.0, octaves=6, persistence=0.5, lacunarity=2.0
):
    offset_x = np.random.uniform(-10_000, 10_000)
    offset_y = np.random.uniform(-10_000, 10_000)

    terrain = np.zeros((height, width))
    cx, cy = width // 2, height // 2

    for i in range(height):
        for j in range(width):
            x = (i - cx) / scale + offset_x
            y = (j - cy) / scale + offset_y
            terrain[i][j] = pnoise2(
                x, y,
                octaves=octaves,
                persistence=persistence,
                lacunarity=lacunarity
            )
    terrain = (terrain - np.min(terrain)) / (np.max(terrain) - np.min(terrain))
    return terrain
    

def falloff_map(width, height):
    cx, cy = width // 2, height // 2
    max_dist = np.sqrt(cx**2 + cy**2)
    falloff = np.zeros((height, width))

    for i in range(height):
        for j in range(width):
            dist = np.sqrt((i - cx) ** 2 + (j - cy) ** 2) / max_dist
            falloff[i, j] = max(0, 1 - dist**2)

    return falloff


def game_map(width, height, spawn_radius=5, **perlin_kwargs):
    falloff = falloff_map(width, height)

    while True:
        terrain = perlin_map(width, height, **perlin_kwargs)
        terrain *= falloff
        terrain = (terrain > 0.35).astype(np.uint8)

        land = np.column_stack(np.where(terrain == 1))
        com_y, com_x = np.mean(land, axis=0)
        com_y, com_x = int(com_y), int(com_x)

        max_x, min_x = com_x + spawn_radius, com_x - spawn_radius
        max_y, min_y = com_y + spawn_radius, com_y - spawn_radius

        if np.all(terrain[min_y : max_y + 1, min_x : max_x : 1] == 1):     
            terrain[min_y: max_y + 1, min_x] = 2
            terrain[min_y: max_y + 1, max_x] = 2
            terrain[min_y, min_x : max_x + 1] = 2
            terrain[max_y, min_x : max_x + 1] = 2

            terrain[com_y, com_x] = 3
            break

        print("Generated invalid spawnpoint; retrying...")
    
    return terrain


if __name__ == "__main__":
    terrain = game_map(200, 200)
    plt.imshow(terrain, cmap=island_colormap())
    plt.axis("off")
    plt.savefig("../data/maps/map.png", bbox_inches='tight', pad_inches=0, dpi=300)
    terrain.tofile("../data/maps/map.bin")


