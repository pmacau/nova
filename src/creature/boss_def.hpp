#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <map/map_system.hpp>

using namespace glm;

struct BossSpawn {
    ivec2 spawnTile;
    bool spawned;
};

inline void initBossSpawnData(std::vector<BossSpawn>& bossSpawns) {
    bossSpawns.clear();
    auto& bossIndices = MapSystem::getBossSpawnIndices();

    for (auto& bossIndex : bossIndices) {
        bossSpawns.push_back({bossIndex, false});
    }
}