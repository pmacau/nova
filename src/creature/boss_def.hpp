#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <map/map_system.hpp>
#include "creature_common.hpp"

using namespace glm;

struct BossSpawn {
    ivec2 spawnTile;
    bool spawned;
    bool defeated;

    CreatureID creatureID;
};

inline void initBossSpawnData(std::vector<BossSpawn>& bossSpawns) {
    bossSpawns.clear();
    auto& bossIndices = MapSystem::getBossSpawnIndices();

    for (auto& bossIndex : bossIndices) {
        bossSpawns.push_back({bossIndex, false, false, CreatureID::BOSS}); // TODO: link to creature def
    }
}