#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <queue>

#include "map/tile.hpp"
#include "util/debug.hpp"
#include "FastNoiseLite.h"


struct NoiseParams {
    double scale = 40.0;
    double persistence = 0.5;
    double lacunarity = 2.0;
    int octaves = 6;
};
 
FastNoiseLite setup_noise(NoiseParams params);
GameMap generate_terrain(int width, int height, NoiseParams params);
Tile discretize(double val);
std::pair<int, int> player_spawn(const GameMap& terrain, int width, int height);

GameMap create_map(int width, int height);
void save_map(const GameMap& map, const char* filepath);