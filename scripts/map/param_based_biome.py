import numpy as np
from enum import Enum
from constants import *
from biome_constants import Biome

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
    Biome.DEFAULT: 0,
    Biome.RAINFOREST: 1,
    Biome.DESERT: 2,
    Biome.FOREST: 3,
    Biome.ICE: 4,
    Biome.VOLCANO: 5,
    Biome.MUSHROOM: 6,
    Biome.CRIMSON: 7,
    Biome.GRAVEYARD: 8,
}

ID_TO_BIOME = {v: k for k, v in BIOME_IDS.items()}

BIOME_RULES = [
    {"type": Biome.RAINFOREST, "temp": Temperature.HIGH, "hum": Humidity.HIGH, "weird": Weirdness.NORMAL},
    {"type": Biome.DESERT, "temp": Temperature.HIGH, "hum": Humidity.LOW, "weird": Weirdness.NORMAL},
    {"type": Biome.FOREST, "temp": Temperature.MID, "hum": Humidity.MID, "weird": Weirdness.NORMAL},
    {"type": Biome.ICE, "temp": Temperature.LOW, "hum": Humidity.LOW, "weird": Weirdness.NORMAL},
    {"type": Biome.VOLCANO, "temp": Temperature.HIGH, "hum": Humidity.LOW, "weird": Weirdness.UNUSUAL},
    {"type": Biome.MUSHROOM, "temp": Temperature.MID, "hum": Humidity.HIGH, "weird": Weirdness.UNUSUAL},
    {"type": Biome.CRIMSON, "temp": Temperature.MID, "hum": Humidity.MID, "weird": Weirdness.UNUSUAL},
    {"type": Biome.GRAVEYARD, "temp": Temperature.LOW, "hum": Humidity.LOW, "weird": Weirdness.UNUSUAL},
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
            return BIOME_IDS[biome["type"]]

    return BIOME_IDS[Biome.DEFAULT]  # Default biome (unassigned)

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
