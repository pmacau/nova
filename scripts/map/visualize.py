from PIL import Image
import matplotlib.pyplot as plt
from map.constants import *

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


def create_coloured_map(terrain, filepath):
    plt.imshow(terrain, cmap="magma")
    plt.axis("off")
    plt.savefig(filepath, bbox_inches='tight', pad_inches=0, dpi=300)


def create_textured_map(terrain, filepath):
    width, height = terrain.shape
    blank = Image.new("RGBA", (TILE_SIZE * (width - 1), TILE_SIZE * (height - 1)), (0, 0, 0, 0))

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
            
            subimage = texture_atlas.crop((
                tile_col * TILE_SIZE,
                tile_row * TILE_SIZE,
                (tile_col + 1) * TILE_SIZE,
                (tile_row + 1) * TILE_SIZE
            ))            
            blank.paste(subimage, (col * TILE_SIZE, row * TILE_SIZE))
    blank.save(filepath)