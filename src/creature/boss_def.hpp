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
	std::cout << "Boss spawn indices size: " << bossIndices.size() << std::endl;
     
    for (auto& bossIndex : bossIndices) {

		if (MapSystem::get_biome_by_indices(bossIndex) == Biome::B_ICE) {
			bossSpawns.push_back({ bossIndex, false, CreatureID::BOSS_BEACH_RED }); 
		}
		else if (MapSystem::get_biome_by_indices(bossIndex) == Biome::B_JUNGLE || MapSystem::get_biome_by_indices(bossIndex) == Biome::B_SAVANNA) {
			bossSpawns.push_back({ bossIndex, false, CreatureID::BOSS_JUNGLE_YELLOW }); 
		}
		else if (MapSystem::get_biome_by_indices(bossIndex) == Biome::B_FOREST) {
			bossSpawns.push_back({ bossIndex, false, CreatureID::BOSS_FOREST_PURPLE }); 
		}
		else {
			bossSpawns.push_back({ bossIndex, false, CreatureID::BOSS });
		}
    }
}