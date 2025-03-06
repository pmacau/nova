#include <vector>
#include <fstream>
#include <iostream>
#include "util/debug.hpp"

std::vector<std::vector<uint8_t>> loadBinaryMap(const std::string& filename, int width, int height) {
    std::vector<std::vector<uint8_t>> game_map(height, std::vector<uint8_t>(width));
    std::ifstream file(filename, std::ios::binary);

    if (file) {
        for (int y = 0; y < height; y++) {
            file.read(reinterpret_cast<char*>(game_map[y].data()), width);
        }
    } else {
        debug_printf(DebugType::WORLD_INIT, "Could not find map file!\n");
    }

    return game_map;
}