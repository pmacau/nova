#pragma once
#include "common.hpp"

/*
{ BBB   |  DDD        | TT }
  Biome |  Decoration | Terrain
*/
using Tile = uint8_t;
const uint8_t TERRAIN_MASK = 0b00000011;
const uint8_t DECOR_MASK   = 0b00011100;
const uint8_t BIOME_MASK   = 0b11100000; 

enum Terrain    {WATER, SAND, GRASS};
enum Decoration {NO_DECOR, SPAWN, BOSS, SHIP, TREE, SHRUB, ROCK};
enum Biome      {B_FOREST, B_BEACH, B_ICE, B_SAVANNA, B_JUNGLE, B_OCEAN};

inline Terrain    get_terrain(Tile tile)    { return static_cast<Terrain>   (tile & TERRAIN_MASK);};
inline Decoration get_decoration(Tile tile) { return static_cast<Decoration>(tile & DECOR_MASK);};
inline Biome      get_biome(Tile tile)      { return static_cast<Biome>     (tile & BIOME_MASK);};

inline void set_terrain(Tile& tile, Terrain t) {
    tile = (tile & ~TERRAIN_MASK) | (static_cast<Tile>(t) & TERRAIN_MASK);
};
inline void set_decoration(Tile& tile, Decoration d) {
    tile = (tile & ~DECOR_MASK) | (static_cast<Decoration>(d) & DECOR_MASK);
};
inline void set_biome(Tile& tile, Biome b) {
    tile = (tile & ~BIOME_MASK) | (static_cast<Tile>(b) & BIOME_MASK);
};

using GameMap = std::vector<std::vector<Tile>>;

const int TILE_SIZE = 16;
const int TILESET_W = 128;
const int TILESET_H = 112;