import numpy as np
from collections import deque
from map.constants import *

def find_mainland(terrain, spawn):
    height, width = terrain.shape

    mainland = np.zeros_like(terrain)
    mainland[*spawn] = 1

    queue = deque([spawn])
    dirs = [(0, 1), (0, -1), (1, 0), (-1, 0)]

    while queue:
        y, x = queue.popleft()
       
        for dx, dy in dirs:
            nx, ny = x + dx, y + dy

            if (
                0 <= nx < width and
                0 <= ny < height and
                mainland[ny, nx] == 0 and
                terrain[ny, nx] != 0
            ):
                mainland[ny, nx] = 1
                queue.append((ny, nx))

    return mainland

def biome_seeds(mainland, spawn, num_seeds):
    valid_indices = np.argwhere(mainland == 1)
    selected = [spawn]

    for _ in range(num_seeds):
        farthest = max(
            valid_indices,
            key=lambda x: min(np.linalg.norm(x - s) for s in selected)
        )
        selected.append(farthest)

    return selected[1:]


def create_biome_map(mainland, seeds):
    height, width = mainland.shape
    biome_map = np.zeros_like(mainland)
    biome_seeds = zip(seeds, range(1, len(seeds) + 1))

    queue = deque(list(biome_seeds))
    for seed, color in biome_seeds:
        biome_map[*seed] = color

    dirs = [(0, 1), (0, -1), (1, 0), (-1, 0)]
    while queue:
        seed, color = queue.popleft()
        y, x = seed

        flip = np.random.rand()
        if (
            (color == 1 and flip < 0.75) or
            (color != 1 and flip < 0.25)
        ):
            queue.append((seed, color))
            continue

        for dx, dy in dirs:
            nx, ny = x + dx, y + dy

            if (
                0 <= nx < width and
                0 <= ny < height and
                biome_map[ny, nx] == 0 and
                mainland[ny, nx] != 0
            ):
                biome_map[ny, nx] = color
                queue.append(((ny, nx), color))
    return biome_map


def place_trees(terrain, num_trees, min_spacing):
    rows, cols = terrain.shape
    tree_positions = []
    grid = terrain.copy()
    
    def is_valid_position(r, c):
        if terrain[r, c] == 0:
            return False
        
        for tr, tc in tree_positions:
            if abs(tr - r) < min_spacing and abs(tc - c) < min_spacing:
                return False
        return True
    
    attempts = 0
    while len(tree_positions) < num_trees and attempts < num_trees * 10:
        r, c = np.random.randint(0, rows), np.random.randint(0, cols)
        if is_valid_position(r, c):
            grid[r, c] = 5 # place tree
            tree_positions.append((r, c))
        attempts += 1
    
    return grid