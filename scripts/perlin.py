import map.visualize as viz
import map.generate as gen
import map.biome as biome
from map.constants import *

MAP_WIDTH = 200
MAP_HEIGHT = 200

if __name__ == "__main__":
    terrain, spawn = gen.game_map(MAP_WIDTH, MAP_HEIGHT, min_land=0.45, scale=50)
    mainland = biome.find_mainland(terrain, spawn)
    boss_spawns = biome.biome_seeds(mainland, spawn, 4)
    for boss in boss_spawns:
        terrain[*boss] = 4
    biome_map = biome.create_biome_map(mainland, [spawn] + boss_spawns)

    terrain.tofile("../data/maps/map.bin")
    viz.create_coloured_map(terrain, "../data/maps/map.png")
    viz.create_coloured_map(biome_map, "../data/maps/biome.png")
    viz.create_textured_map(terrain, "../data/maps/textured_map.png")


