import os
import shutil
import argparse
from PIL import Image
from math import pow, sqrt

PATH_TO_DATA = os.path.join(os.path.dirname(os.getcwd()), "data")
PATH_TO_PALETTES = os.path.join(PATH_TO_DATA, "palettes")
PATH_TO_TEXTURES = os.path.join(PATH_TO_DATA, "textures")
PATH_TO_RETEXTURES = os.path.join(PATH_TO_DATA, "retextures")

class RetextureException(Exception):
    ...

def setup_cli():
    parser = argparse.ArgumentParser(description="A script to retexture sprites with a specified palette")
    parser.add_argument(
        "palette", type=str, nargs="?", const=True, default="resurrect-64.hex",
        help="The palette file name to rexture the sprites (in ../data/palettes/)"
    )
    return parser.parse_args()

def hex_to_rgb(hex_str):
    try:
        r = int(hex_str[0:2], 16)
        g = int(hex_str[2:4], 16)
        b = int(hex_str[4:6], 16)
    except Exception as e:
        raise RetextureException("Error parsing the palette. It must be an RGB .hex file", e)
    return (r, g, b)

def parse_palette(file_path: str):
    with open(file_path, "r") as f:
        colors = [hex_to_rgb(line.strip()) for line in f.readlines()]
    return colors

def copy_textures():
    if not os.path.isdir(PATH_TO_TEXTURES):
        raise(RetextureException("Path to source textures is not a directory."))
    
    if os.path.exists(PATH_TO_RETEXTURES):
        shutil.rmtree(PATH_TO_RETEXTURES)

    os.makedirs(PATH_TO_RETEXTURES, exist_ok=True)
    for item in os.listdir(PATH_TO_TEXTURES):
        src = os.path.join(PATH_TO_TEXTURES, item)
        dst = os.path.join(PATH_TO_RETEXTURES, item)

        if os.path.isdir(src):
            shutil.copytree(src, dst, dirs_exist_ok=True)
        else:
            shutil.copy2(src, dst)

def retexture_all_files(palette: list):
    try:
        for dirpath, _, filenames in os.walk(PATH_TO_RETEXTURES):
            for filename in filenames:
                if filename.lower().endswith('.png'):
                    file_path = os.path.join(dirpath, filename)
                    retexture(file_path, palette)
    except Exception as e:
        raise RetextureException("An error occurred while retexturing. Aborting.", e)

def retexture(file_path: str, palette: list):
    image = Image.open(file_path).convert("RGBA")
    width, height = image.size
    pixels = image.load()

    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            if a != 0:
                pixels[x, y] = closest_color((r, g, b, a), palette)

    image.save(file_path)


def closest_color(color: tuple, palette: list):
    min_dist = float("inf")
    best_color = color
    r, g, b, a = color

    for p_r, p_g, p_b in palette:
        color_dist = sqrt(pow(r - p_r, 2) + pow(g - p_g, 2) + pow(b - p_b, 2))
        if color_dist < min_dist:
            min_dist = color_dist
            best_color = (p_r, p_g, p_b, a)

    return best_color


if __name__ == "__main__":
    if  not os.path.exists(PATH_TO_DATA) or \
        not os.path.exists(PATH_TO_PALETTES) or \
        not os.path.exists(PATH_TO_TEXTURES):
        raise(
            RetextureException(
                "Incorrect directory structure: ../data/palettes/ and ../data/textures/ must exist."
            )
        )
    args = setup_cli()
    palette = parse_palette(os.path.join(PATH_TO_PALETTES, args.palette))
    copy_textures()
    retexture_all_files(palette)




    
