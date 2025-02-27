import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from noise import pnoise2
from PIL import Image
from collections import deque

tile_char = {
    0: "W", 1: "S", 2: "G"
}

keys = [
    ["GGGG", "GGGW", "GGWG", "GWGG", "WGGG", "SSGG", "SGSG", "WWGS"],
    ["WWWW", "WWWG", "WWGW", "WGWW", "GWWW", "WWSS", "SSWW", "WWSG"],
    ["SSSS", "SSSW", "SSWS", "SWSS", "WSSS", "WGWG", "GWGW", "SWGW"],
    ["GGGS", "GGSG", "GSGG", "SGGG", "GGSS", "WSWS", "SWSW", "GWSW"],
    ["WWWS", "WWSW", "WSWW", "SWWW", "WWGG", "GSGS", "GGWW", "WSWG"],
    ["SSSG", "SSGS", "SGSS", "GSSS", "WSSW", "SWWS", "GSSG", "WGWS"],
    ["SGGS", "GSSW", "SGWS", "SWGS", "WSSG", "SGWW", "GSWW", "RRRR"]
]
h, w = len(keys), len(keys[0])
texture_map = {
    keys[r][c] : (r, c) for r in range(h) for c in range(w)
}

def island_colormap():
    colors = [(0, 0, 1), (1, 1, 0), (0, 1, 0), (0, 0, 0)]
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

        bins = [0, 0.33, 0.4, 1]
        terrain = (np.digitize(terrain, bins) - 1).astype(np.uint8)

        land = np.column_stack(np.where(terrain > 0))
        com_y, com_x = np.mean(land, axis=0)
        com_y, com_x = int(com_y), int(com_x)

        max_x, min_x = com_x + spawn_radius, com_x - spawn_radius
        max_y, min_y = com_y + spawn_radius, com_y - spawn_radius

        if np.all(terrain[min_y : max_y + 1, min_x : max_x : 1] > 0):
            terrain[com_y, com_x] = 3
            print("Found valid spawnpoint; map generated!")
            break

        print("Generated invalid spawnpoint; retrying...")
    
    return np.pad(terrain, 1, mode="constant", constant_values=0), (com_y, com_x)


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


def get_boss_spawns(mainland, spawn, num_bosses):
    valid_indices = np.argwhere(mainland == 1)
    selected = [spawn]

    for _ in range(num_bosses):
        farthest = max(
            valid_indices,
            key=lambda x: min(np.linalg.norm(x - s) for s in selected)
        )
        selected.append(farthest)

    return selected[1:]



def create_textured_map(terrain):
    width, height = terrain.shape
    blank = Image.new("RGBA", (16 * (width - 1), 16 * (height - 1)), (0, 0, 0, 0))

    texture_atlas = Image.open("../data/textures/tile/tileset.png")
    for row in range(height - 1):
        for col in range(width - 1):
            tile_str = (
                tile_char.get(terrain[row][col], "R") +
                tile_char.get(terrain[row][col + 1], "R") +
                tile_char.get(terrain[row + 1][col], "R") + 
                tile_char.get(terrain[row + 1][col + 1], "R")
            )
            tile_row, tile_col = texture_map.get(tile_str, (h-1, w-1))
            
            subimage = texture_atlas.crop(
                (tile_col * 16, tile_row * 16, (tile_col + 1) * 16, (tile_row + 1) * 16)
            )            
            blank.paste(subimage, (col * 16, row * 16))
    blank.save("../data/maps/textured_map.png")


if __name__ == "__main__":
    terrain, spawn = game_map(
        198, 198,
        **{"scale": 50.0}
    )
    mainland = find_mainland(terrain, spawn)
    boss_spawns = get_boss_spawns(mainland, spawn, 4)
    for boss in boss_spawns:
        terrain[*boss] = 4

    print(terrain.shape)

    plt.imshow(terrain, cmap="magma")
    plt.axis("off")
    plt.savefig("../data/maps/map.png", bbox_inches='tight', pad_inches=0, dpi=300)
    terrain.tofile("../data/maps/map.bin")
    create_textured_map(terrain)


