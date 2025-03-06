#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <entt.hpp>

#include "common.hpp"
#include "util/debug.hpp"

enum class Tile {
    WATER, 
    SAND, 
    GRASS,
    SPAWN, 
    BOSS_SPAWN,
    TREE,
    TILE_COUNT
};

class MapSystem {
public:
    static void init(entt::registry& reg);
    static void generate_new_map();
    static vec2 populate_ecs(entt::registry& reg);
    static void update_location(entt::registry& reg, entt::entity ent);
private:
    static constexpr int MAP_WIDTH = 200;
    static constexpr int MAP_HEIGHT = 200;
    static constexpr int TILE_SIZE = 16; // tile size in pixels
    static inline std::vector<std::vector<uint8_t>> game_map;

    static void loadMap();
    static Tile get_tile(vec2 pos);
    static bool walkable_tile(Tile tile);
};