#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <map/map_system.hpp>
#include "creature_common.hpp"

using namespace glm;

struct BossSpawn {
    ivec2 spawnTile;
    bool spawned;

    CreatureID creatureID;
};

inline void initBossSpawnData(std::vector<BossSpawn>& bossSpawns) {
    bossSpawns.clear();
    auto& bossIndices = MapSystem::getBossSpawnIndices();
     
    for (auto& bossIndex : bossIndices) {

		if (MapSystem::get_biome_by_indices(bossIndex) == Biome::B_ICE) {
			bossSpawns.push_back({ bossIndex, false, CreatureID::BOSS_BEACH_RED }); 
			//std::cout << "1Boss spawn: " << bossIndex.x << ", " << bossIndex.y << std::endl;
		}
		else if (MapSystem::get_biome_by_indices(bossIndex) == Biome::B_SAVANNA) {
			bossSpawns.push_back({ bossIndex, false, CreatureID::BOSS_JUNGLE_YELLOW }); 
			//std::cout << "2Boss spawn: " << bossIndex.x << ", " << bossIndex.y << std::endl;
		}
		else if (MapSystem::get_biome_by_indices(bossIndex) == Biome::B_JUNGLE) {
			bossSpawns.push_back({ bossIndex, false, CreatureID::BOSS_FOREST_PURPLE }); 
			//std::cout << "3Boss spawn: " << bossIndex.x << ", " << bossIndex.y << std::endl;

		}
		else {
			bossSpawns.push_back({ bossIndex, false, CreatureID::BOSS });
			//std::cout << "4Boss spawn: " << bossIndex.x << ", " << bossIndex.y << std::endl;
		}
    }
}