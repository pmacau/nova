import numpy as np
from noise import pnoise2
from map.constants import *

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


def game_map(width, height, spawn_radius=5, min_land=0, **perlin_kwargs):
    falloff = falloff_map(width - 2, height - 2)

    while True:
        terrain = perlin_map(width - 2, height - 2, **perlin_kwargs)
        terrain *= falloff

        bins = [0, 0.33, 0.4, 1]
        terrain = (np.digitize(terrain, bins) - 1).astype(np.uint8)

        if (np.count_nonzero(terrain) / (width * height)) < min_land:
            continue

        land = np.column_stack(np.where(terrain > 0))
        com_y, com_x = np.mean(land, axis=0)
        com_y, com_x = int(com_y), int(com_x)

        max_x, min_x = com_x + spawn_radius, com_x - spawn_radius
        max_y, min_y = com_y + spawn_radius, com_y - spawn_radius

        if np.all(terrain[min_y : max_y + 1, min_x : max_x : 1] > 0):
            terrain[com_y, com_x] = 3
            break
    
    return np.pad(terrain, 1, mode="constant", constant_values=0), (com_y, com_x)