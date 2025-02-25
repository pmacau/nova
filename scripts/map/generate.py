import numpy as np
from noise import pnoise2
from constants import *


def generate_noise_map(width, height, scale=50.0, octaves=6, persistence=0.5, lacunarity=2.0, normalize=True):
    """
    Generate a Perlin noise map, normalized between -1 and 1 (or 0 and 1).
    """
    offset_x = np.random.uniform(-100_000, 100_000)
    offset_y = np.random.uniform(-100_000, 100_000)

    noise_map = np.zeros((height, width))
    
    for i in range(height):
        for j in range(width):
            x = i / scale + offset_x
            y = j / scale + offset_y
            noise_map[i][j] = pnoise2(
                x, y,
                octaves=octaves,
                persistence=persistence,
                lacunarity=lacunarity
            )

    if normalize:
        min_val, max_val = np.min(noise_map), np.max(noise_map)
        noise_map = 2 * ((noise_map - min_val) / (max_val - min_val)) - 1  # Normalize to [-1, 1]

    return noise_map

def perlin_map(
        width, height,
        scale=50.0, octaves=6, persistence=0.5, lacunarity=2.0
):
    offset_x = np.random.uniform(-100_000, 100_000)
    offset_y = np.random.uniform(-100_000, 100_000)

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


def game_map(
        width, height,
        spawn_radius=5, water_padding=1,
        bins=[0, 0.33, 0.4, 1],
        terrain_map_perlin_params = {},
        temp_map_perlin_params = {},
        hum_map_perlin_params = {},
        weird_map_perlin_params = {}
):
    falloff = falloff_map(width, height)

    while True:
        terrain = perlin_map(width, height, **terrain_map_perlin_params)
        terrain *= falloff

        terrain = (np.digitize(terrain, bins) - 1).astype(np.uint8)

        land = np.column_stack(np.where(terrain > 0))
        com_y, com_x = np.mean(land, axis=0)
        com_y, com_x = int(com_y), int(com_x)

        max_x, min_x = com_x + spawn_radius, com_x - spawn_radius
        max_y, min_y = com_y + spawn_radius, com_y - spawn_radius

        if np.all(terrain[min_y : max_y + 1, min_x : max_x : 1] > 0):
            break
        print("Generated invalid spawnpoint; retrying...")

    terrain[com_y, com_x] = SPAWN
    terrain = np.pad(terrain, pad_width=water_padding, constant_values=0)

    # Generate biome factor noise maps
    temperature_map = generate_noise_map(width, height, **temp_map_perlin_params)
    humidity_map = generate_noise_map(width, height, **hum_map_perlin_params)
    weirdness_map = generate_noise_map(width, height, **weird_map_perlin_params)

    return terrain, (com_x, com_y), temperature_map, humidity_map, weirdness_map
        