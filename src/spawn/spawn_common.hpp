#pragma once
#include <map>
#include <map/tile.hpp>


const float NIGHT_SPAWN_TIME_MULTIPLIER = 0.9f;
const float NIGHT_SPAWN_CAP_MULTIPLIER = 1.5f;

struct SpawnData {
    size_t spawnCap;
    float spawnTimeInterval;
};

const std::map<Biome, SpawnData> biomeSpawnData = {
    {Biome::B_FOREST, {10, 5000.0f}},
    {Biome::B_BEACH, {10, 5000.0f}},
    {Biome::B_ICE, {10, 5000.0f}},
    {Biome::B_JUNGLE, {10, 5000.0f}},
    {Biome::B_SAVANNA, {12, 5000.0f}},
    {Biome::B_OCEAN, {10, 5000.0f}}
};