import generate as gen
import param_based_biome as biome
import visualize_param_biome as viz

WIDTH = 1000
HEIGHT = 1000

if __name__ == "__main__":
    terrain_map_perlin_params = {
        "scale": 150.0,
        "octaves": 6,
        "persistence": 0.5,
        "lacunarity": 2.0
    }

    temp_map_perlin_params = {
        "scale": 400.0,       # Larger scale = smoother, bigger temperature zones
        "octaves": 5,         # Keeps enough detail without creating artifacts
        "persistence": 0.5,   # Balanced detail intensity per octave
        "lacunarity": 2.0     # Standard frequency increase, prevents artifacts
    }

    hum_map_perlin_params = {
        "scale": 350.0,       
        "octaves": 5,         
        "persistence": 0.5,   
        "lacunarity": 2.0     
    }

    weird_map_perlin_params = {
        "scale": 250.0,       
        "octaves": 6,         
        "persistence": 0.5,   
        "lacunarity": 2.0     
    }


    terrain, spawn, temp_map, hum_map, weird_map = gen.game_map(
        WIDTH, HEIGHT, 
        terrain_map_perlin_params=terrain_map_perlin_params,
        temp_map_perlin_params=temp_map_perlin_params,
        hum_map_perlin_params=hum_map_perlin_params, 
        weird_map_perlin_params=weird_map_perlin_params
    )

    biome_map = biome.generate_biomes(WIDTH, HEIGHT, temp_map, hum_map, weird_map)

    viz.visualize_all_maps(biome_map, terrain, temp_map, hum_map, weird_map)