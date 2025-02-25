import numpy as np
import random
from collections import deque
from constants import *

def far_apart_seeds(map_data, num_biomes):
    height, width = map_data.shape
    seeds = []
    
    # Approximate grid spacing for seed placement
    step_x = width // int(np.sqrt(num_biomes))
    step_y = height // int(np.sqrt(num_biomes))

    # Spread seeds in a grid-like fashion
    for i in range(num_biomes):
        x = (i % int(np.sqrt(num_biomes))) * step_x + step_x // 2
        y = (i // int(np.sqrt(num_biomes))) * step_y + step_y // 2

        # Find the closest valid land position
        for _ in range(100):  # Try 100 random nearby points to find land
            nx, ny = min(width - 1, max(0, x + random.randint(-step_x//3, step_x//3))), \
                     min(height - 1, max(0, y + random.randint(-step_y//3, step_y//3)))
            if map_data[ny, nx] != WATER:
                seeds.append((nx, ny, i + 1))
                break
    
    return seeds

def generate_biomes(map_data, seed_positions):
    height, width = map_data.shape
    biome_map = np.zeros_like(map_data)

    # Assign initial biome seeds
    for x, y, biome in seed_positions:
        biome_map[y, x] = biome

    # Region growing (BFS)
    queue = deque(seed_positions)
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]  # Up, Down, Left, Right

    while queue:
        x, y, biome = queue.popleft()
        
        for dx, dy in directions:
            nx, ny = x + dx, y + dy
            
            if 0 <= nx < width and 0 <= ny < height:  # Stay within bounds
                if biome_map[ny, nx] == 0 and map_data[ny, nx] != WATER:  # Unassigned & not water
                    biome_map[ny, nx] = biome
                    queue.append((nx, ny, biome))
    
    return biome_map
