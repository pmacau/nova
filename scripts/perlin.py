import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from noise import pnoise2
from PIL import Image

num_to_tile_char = {
    0: "W", 1: "S", 2: "G", 3: "R"
}

keys = [
    "GGGG", "GGGW", "GGWG", "GWGG", "WGGG", "SSGG", "SGSG",
    "WWWW", "WWWG", "WWGW", "WGWW", "GWWW", "WWSS", "SSWW",
    "SSSS", "SSSW", "SSWS", "SWSS", "WSSS", "WGWG", "GWGW",
    "GGGS", "GGSG", "GSGG", "SGGG", "GGSS", "WSWS", "SWSW",
    "WWWS", "WWSW", "WSWW", "SWWW", "WWGG", "GSGS", "GGWW",
    "SSSG", "SSGS", "SGSS", "GSSS", "WSSW", "SWWS", "GSSG",
    "SGGS", "GSSW", "SGWS", "SWGS", "WSSG", "RRRR"
]
texture_map = {}
for row in range(7):
    for col in range(7):
        idx = row * 7 + col
        if idx >= len(keys):
            continue
        texture_map[keys[idx]] = (row, col)

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
            terrain[min_y: max_y + 1, min_x] = 3
            terrain[min_y: max_y + 1, max_x] = 3
            terrain[min_y, min_x : max_x + 1] = 3
            terrain[max_y, min_x : max_x + 1] = 3

            terrain[com_y, com_x] = 3
            break

        print("Generated invalid spawnpoint; retrying...")
    
    return terrain


def create_textured_map(terrain):
    width, height = terrain.shape
    blank = Image.new("RGBA", (16 * (width - 1), 16 * (height - 1)), (0, 0, 0, 0))

    texture_atlas = Image.open("../data/textures/tile/tileset.png")
    for row in range(height - 1):
        for col in range(width - 1):
            tile_str = num_to_tile_char.get(terrain[row][col]) + \
                       num_to_tile_char.get(terrain[row][col + 1]) + \
                       num_to_tile_char.get(terrain[row + 1][col]) + \
                       num_to_tile_char.get(terrain[row + 1][col + 1])
            tile_row, tile_col = texture_map.get(tile_str, (6, 5))
            
            subimage = texture_atlas.crop(
                (tile_col * 16, tile_row * 16, (tile_col + 1) * 16, (tile_row + 1) * 16)
            )            
            blank.paste(subimage, (col * 16, row * 16))
    blank.save("../data/maps/textured_map.png")


if __name__ == "__main__":
    terrain = game_map(200, 200)

    plt.imshow(terrain, cmap=island_colormap())
    plt.axis("off")
    plt.savefig("../data/maps/map.png", bbox_inches='tight', pad_inches=0, dpi=300)
    terrain.tofile("../data/maps/map.bin")

    create_textured_map(terrain)


