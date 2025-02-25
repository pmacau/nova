from PIL import Image
import matplotlib.pyplot as plt
import numpy as np
from constants import *
from param_based_biome import BIOME_IDS

# BIOme colors
BIOME_COLORS = {
    1: (34, 139, 34, 255),   # Rainforest (Green)
    2: (210, 180, 140, 255),  # Desert (Tan)
    3: (60, 179, 113, 255),   # Forest (Dark Green)
    4: (173, 216, 230, 255),  # Ice Biome (Light Blue)
    5: (255, 69, 0, 255),     # Volcano (Red-Orange)
    6: (147, 112, 219, 255),  # Mushroom Biome (Purple)
    7: (220, 20, 60, 255),    # Crimson Biome (Dark Red)
    8: (105, 105, 105, 255),  # Graveyard (Dark Gray)
    0: (255, 255, 255, 255),  # Default/Unassigned (White)
}

TERRAIN_COLORS = {
    WATER: (0, 0, 255, 255),  # Water (Blue)
    SAND: (255, 255, 102, 255),  # Sand (Yellow)
    GRASS: None,  # Land (Biome-colored)
}

def visualize_biome_with_terrain(biome_map, terrain_map, filename="biome_terrain_map.png", show=False):
    """
    Visualizes the biome map with terrain features:
    - Water = Blue
    - Sand = Yellow
    - Land = Biome-colored
    """
    height, width = biome_map.shape
    img = Image.new("RGBA", (width, height))

    for y in range(height):
        for x in range(width):
            terrain_type = terrain_map[y, x]

            if terrain_type in TERRAIN_COLORS and TERRAIN_COLORS[terrain_type] is not None:
                img.putpixel((x, y), TERRAIN_COLORS[terrain_type])  # Water/Sand coloring
            else:
                biome_id = biome_map[y, x]
                img.putpixel((x, y), BIOME_COLORS.get(biome_id, (255, 255, 255, 255)))  # Land gets biome color

    img.save(f"{DATA_PATH}/maps/{filename}")
    if show:
        img.show()

def visualize_biome_map(biome_map, filename="biome_map.png", show=False):
    """
    Visualizes the biome map using predefined colors.
    """
    height, width = biome_map.shape
    img = Image.new("RGBA", (width, height))

    # Assign biome colors
    for y in range(height):
        for x in range(width):
            biome_id = biome_map[y, x]
            img.putpixel((x, y), BIOME_COLORS.get(biome_id, (255, 255, 255, 255)))

    img.save(f"{DATA_PATH}/maps/{filename}")
    if show:
        img.show()

def visualize_noise_map(noise_map, cmap, filename="noise_map.png", show=False):
    """
    Visualizes a noise map (temperature, humidity, weirdness).
    """
    plt.imshow(noise_map, cmap=cmap)
    plt.axis("off")

    plt.savefig(f"{DATA_PATH}/maps/{filename}", bbox_inches='tight', pad_inches=0, dpi=300)
    if show:
        plt.show()

def visualize_all_maps(biome_map, terrain_map, temp_map, hum_map, weird_map):
    """
    Generates and saves all visualizations: Biome, Temperature, Humidity, and Weirdness.
    """
    visualize_biome_with_terrain(biome_map, terrain_map, "biome_terrain_map.png")
    visualize_biome_map(biome_map, "biome_map.png")
    visualize_noise_map(temp_map, "coolwarm", "temperature_map.png")
    visualize_noise_map(hum_map, "Blues", "humidity_map.png")
    visualize_noise_map(weird_map, "Purples", "weirdness_map.png")

    print("Maps saved in /data/maps/")
