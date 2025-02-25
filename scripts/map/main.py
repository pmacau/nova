import generate as gen
import visualize as viz
import biome

if __name__ == "__main__":
    terrain, spawn = gen.game_map(
        1000, 1000,
        **{
            "scale": 150.0,
            "octaves":6,
            "persistence":0.5,
            "lacunarity":2.0
        }
    )
    num_biomes = 4
    biome_seeds = [*biome.far_apart_seeds(terrain, 4), (*spawn, num_biomes + 1)]
    biome_map = biome.generate_biomes(terrain, biome_seeds)

    #terrain.tofile("../data/maps/map2.bin")
    viz.create_coloured_map(biome_map, "bone", "map2")
    viz.create_textured_map(terrain, "textured_map2")
    #viz.create_coloured_map(terrain, "bone", "map3")
    #viz.create_textured_map(terrain, "textured_map2")