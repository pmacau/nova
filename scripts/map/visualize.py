from PIL import Image
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from constants import *

num_to_tile_char = {
    WATER: "W", SAND: "S", GRASS: "G", SPAWN: "R"
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


def create_textured_map(terrain, map_name="textured_map"):
    width, height = terrain.shape
    blank = Image.new(
        "RGBA",
        (TILE_DIM * (width - 1), TILE_DIM * (height - 1)),
        (0, 0, 0, 0)
    )

    texture_atlas = Image.open(f"{DATA_PATH}/textures/tile/tileset.png")
    for row in range(height - 1):
        for col in range(width - 1):
            tile_str = num_to_tile_char.get(terrain[row][col]) + \
                       num_to_tile_char.get(terrain[row][col + 1]) + \
                       num_to_tile_char.get(terrain[row + 1][col]) + \
                       num_to_tile_char.get(terrain[row + 1][col + 1])
            tile_row, tile_col = texture_map.get(tile_str, (6, 5))
            
            subimage = texture_atlas.crop((
                tile_col * TILE_DIM,
                tile_row * TILE_DIM,
                (tile_col + 1) * TILE_DIM,
                (tile_row + 1) * TILE_DIM
            ))            
            blank.paste(subimage, (col * 16, row * 16))
    blank.save(f"{DATA_PATH}/maps/{map_name}.png")

def create_coloured_map(terrain_map, cmap, map_name="map", show=False):
    plt.imshow(terrain_map, cmap=cmap)
    plt.axis("off")

    if show:
        plt.show()
    plt.savefig(f"{DATA_PATH}/maps/{map_name}.png", bbox_inches='tight', pad_inches=0, dpi=300)