#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "FastNoiseLite.h"

std::vector<std::vector<double>> perlin_map(
    int width, int height, double scale = 50.0,
    int octaves = 6, double persistence = 0.5, double lacunarity = 2.0
) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-10000, 10000);
    double offset_x = dist(gen);
    double offset_y = dist(gen);

    std::vector<std::vector<double>> terrain(height, std::vector<double>(width, 0.0));

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(1.0 / 40.0);

    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(octaves);
    noise.SetFractalGain(persistence);
    noise.SetFractalLacunarity(lacunarity);

    double min_val = 1e9, max_val = -1e9;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double val = noise.GetNoise(
                float(j + offset_x),
                float(i + offset_y)
            );
            terrain[i][j] = 0.5 * (val + 1.0);
        }
    }
    return terrain;
}

std::vector<std::vector<double>> falloff_map(int width, int height) {
    int cx = width / 2, cy = height / 2;
    double max_dist = sqrt(cx * cx + cy * cy);
    std::vector<std::vector<double>> falloff(height, std::vector<double>(width, 0.0));
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double dist = sqrt((i - cx) * (i - cx) + (j - cy) * (j - cy)) / max_dist;
            falloff[i][j] = std::max(0.0, 1.0 - dist * dist);
        }
    }
    
    return falloff;
}

std::vector<std::vector<uint8_t>> game_map(int width, int height, int spawn_radius = 5, double min_land = 0) {
    std::vector<std::vector<double>> falloff = falloff_map(width - 2, height - 2);
    std::vector<std::vector<uint8_t>> terrain;
    std::vector<std::pair<int, int>> land;
    
    while (true) {
        terrain.assign(height - 2, std::vector<uint8_t>(width - 2, 0));
        land.clear();
        
        std::vector<std::vector<double>> perlin = perlin_map(width - 2, height - 2);
        for (int i = 0; i < height - 2; ++i) {
            for (int j = 0; j < width - 2; ++j) {
                perlin[i][j] *= falloff[i][j];
            }
        }
        
        double bins[] = {0, 0.33, 0.4, 1};
        for (int i = 0; i < height - 2; ++i) {
            for (int j = 0; j < width - 2; ++j) {
                if      (perlin[i][j] < bins[1]) terrain[i][j] = 0;
                else if (perlin[i][j] < bins[2]) terrain[i][j] = 1;
                else terrain[i][j] = 2;
            }
        }
        break;
    }
    
    std::vector<std::vector<uint8_t>> padded(height, std::vector<uint8_t>(width, 0));
    for (int i = 0; i < height - 2; ++i) {
        for (int j = 0; j < width - 2; ++j) {
            padded[i + 1][j + 1] = terrain[i][j];
        }
    }
    
    return padded;
}

void save_map_as_binary(const std::vector<std::vector<uint8_t>>& map, const char* filepath) {
    std::ofstream file(filepath, std::ios::binary);
    int height = map.size();
    int width = map[0].size();
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    for (const auto& row : map) {
        file.write(reinterpret_cast<const char*>(row.data()), row.size());
    }
    file.close();
}

void save_double_map_as_binary(const std::vector<std::vector<double>>& map, const char* filepath) {
    std::ofstream outfile(filepath);

    for (const auto& row : map) {
        for (const auto& value : row) {
            outfile << value << " ";
        }
        outfile << std::endl;
    }
    outfile.close();
}