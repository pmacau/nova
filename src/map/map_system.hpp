#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <entt.hpp>

#include "map/tile.hpp"
#include "common.hpp"
#include "util/debug.hpp"

class MapSystem {
public:
    static void init(entt::registry& reg);
    static void generate_new_map();
    static vec2 populate_ecs(entt::registry& reg);
    static void update_location(entt::registry& reg, entt::entity ent);
private:
    static constexpr int MAP_WIDTH = 200;
    static constexpr int MAP_HEIGHT = 200;
    static inline GameMap game_map;

    static void loadMap();
    static Tile get_tile(vec2 pos);
    static bool walkable_tile(Tile tile);
};