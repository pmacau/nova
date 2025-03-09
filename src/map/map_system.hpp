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
    static vec2 populate_ecs(entt::registry& reg, vec2& p_pos, vec2& s_pos);
    static void update_location(entt::registry& reg, entt::entity ent);

    inline static int map_width  = -1;
    inline static int map_height = -1;

    static Tile get_tile(vec2 pos);
    static Tile get_tile_type_by_indices(int x, int y);
    static vec2 get_tile_indices(vec2 pos);
    static vec2 get_tile_center_pos(vec2 tile_indices);
    static bool walkable_tile(Tile tile);

    static std::vector<vec2>& getBossSpawnIndices();

private:
    static inline GameMap game_map;

    static void loadMap();

    static std::vector<vec2> bossSpawnIndices;
};