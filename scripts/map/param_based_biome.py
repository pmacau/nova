import numpy as np
from enum import Enum
from constants import *

# Define categories for noise levels
class Temperature(Enum):
    LOW = "Low"
    MID = "Mid"
    HIGH = "High"

class Humidity(Enum):
    LOW = "Low"
    MID = "Mid"
    HIGH = "High"

class Weirdness(Enum):
    NORMAL = "Normal"
    UNUSUAL = "Unusual"
    EXTREME = "Extreme"  # Edge cases if applicable

def classify_temperature(value):
    if value < -0.5:
        return Temperature.LOW
    elif value > 0.5:
        return Temperature.HIGH
    return Temperature.MID

def classify_humidity(value):
    if value < -0.5:
        return Humidity.LOW
    elif value > 0.5:
        return Humidity.HIGH
    return Humidity.MID

def classify_weirdness(value):
    if -0.5 <= value <= 0.5:
        return Weirdness.NORMAL
    elif -0.9 <= value <= 0.9:
        return Weirdness.UNUSUAL  
    return Weirdness.EXTREME

BIOME_IDS = {
    "Default": 0,
    "Rainforest": 1,
    "Desert": 2,
    "Forest": 3,
    "Ice Biome": 4,
    "Volcano": 5,
    "Mushroom Biome": 6,
    "Crimson Biome": 7,
    "Graveyard": 8,
}

BIOME_RULES = [
    {"name": "Rainforest", "temp": Temperature.HIGH, "hum": Humidity.HIGH, "weird": Weirdness.NORMAL},
    {"name": "Desert", "temp": Temperature.HIGH, "hum": Humidity.LOW, "weird": Weirdness.NORMAL},
    {"name": "Forest", "temp": Temperature.MID, "hum": Humidity.MID, "weird": Weirdness.NORMAL},
    {"name": "Ice Biome", "temp": Temperature.LOW, "hum": Humidity.LOW, "weird": Weirdness.NORMAL},
    {"name": "Volcano", "temp": Temperature.HIGH, "hum": Humidity.LOW, "weird": Weirdness.UNUSUAL},
    {"name": "Mushroom Biome", "temp": Temperature.MID, "hum": Humidity.HIGH, "weird": Weirdness.UNUSUAL},
    {"name": "Crimson Biome", "temp": Temperature.MID, "hum": Humidity.MID, "weird": Weirdness.UNUSUAL},
    {"name": "Graveyard", "temp": Temperature.LOW, "hum": Humidity.LOW, "weird": Weirdness.UNUSUAL},
]

def classify_biome(temp, hum, weird):
    """
    Dynamically determines the biome type using BIOME_RULES.
    """
    temp_category = classify_temperature(temp)
    hum_category = classify_humidity(hum)
    weird_category = classify_weirdness(weird)

    for biome in BIOME_RULES:
        if (
            biome["temp"] == temp_category and
            biome["hum"] == hum_category and
            biome["weird"] == weird_category
        ):
            return BIOME_IDS[biome["name"]]

    return 0  # Default biome (unassigned)

def generate_biomes(width, height, temp_map, hum_map, weird_map):
    """
    Assigns biomes dynamically using temperature, humidity, and weirdness maps.
    """
    biome_map = np.zeros((height, width), dtype=np.uint8)

    for y in range(height):
        for x in range(width):
            temp = temp_map[y, x]
            hum = hum_map[y, x]
            weird = weird_map[y, x]
            biome_map[y, x] = classify_biome(temp, hum, weird)

    return biome_map
