#include <iostream>
#include <cmath>
#include "spawn_system.hpp"
#include "tinyECS/components.hpp"
#include "common.hpp"
#include "world_init.hpp"
#include "util/debug.hpp"



SpawnSystem::SpawnSystem(entt::registry &registry)
    : registry(registry)
{
    std::cout << "SpawnSystem initialized." << std::endl;

    // random seed.
    rng.seed(std::random_device()());
}

SpawnSystem::~SpawnSystem()
{

}

void SpawnSystem::update(float deltaTime) {
    spawnTimer += deltaTime;
    
    // Check spawn cap
    size_t currentMobCount = registry.view<Mob>().size();
    if (currentMobCount >= spawnCap) {
        // std::cout << "Spawn cap reached (" << currentMobCount << " mobs). Skipping spawn." << std::endl;
    } else if (spawnTimer >= spawnRate) {
        // attempt spawning.
        spawnTimer = 0.0f; 
        processSpawning();
    }
    
    processDespawning();
}


void SpawnSystem::processSpawning() {
    // Get the player entity
    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0) {
		debug_printf(DebugType::SPAWN, "No player entity found (spawn)\n");
        return;  // No player found.
    }
    auto playerEntity = *playerView.begin();
    auto& playerMotion = registry.get<Motion>(playerEntity);
    vec2 playerPos = playerMotion.position;
    
    // Generate a random spawn position
    std::uniform_real_distribution<float> angleDist(0.0f, 2 * 3.14159f);
    std::uniform_real_distribution<float> distanceDist(SAFE_ZONE_RADIUS, SPAWN_ZONE_RADIUS);
    float randomAngle = angleDist(rng);
    float randomDistance = distanceDist(rng);
    
    vec2 candidatePos = { 
        playerPos.x + randomDistance * std::cos(randomAngle),
        playerPos.y + randomDistance * std::sin(randomAngle)
    };

    // Squared distance check (for later extensions: might want to use box2d and other checks)
    float dx = candidatePos.x - playerPos.x;
    float dy = candidatePos.y - playerPos.y;
    float sqDistance = dx * dx + dy * dy;
    if (sqDistance < (SAFE_ZONE_RADIUS * SAFE_ZONE_RADIUS) ||
        sqDistance > (SPAWN_ZONE_RADIUS * SPAWN_ZONE_RADIUS)) {
        std::cout << "Candidate position out of range." << std::endl;
        return;
    }
    
    // Assume candidate's biome is 0 for now
    // TODO: biome system
    int candidateBiome = 0;
    
    // Get all eligible spawn definitions for the given location
    std::vector<const SpawnDefinition*> eligibleDefs;
    for (const auto& def : spawnDefinitions) {
        bool allowedBiome = false;
        for (int biome : def.biomeIDs) {
            if (biome == candidateBiome) {
                allowedBiome = true;
                break;
            }
        }
        if (allowedBiome) {
            eligibleDefs.push_back(&def);
        }
    }
    
    if (eligibleDefs.empty()) {
		debug_printf(DebugType::SPAWN, "No eligible spawn definitions for biome \n");
        return;
    }
    
    // Calculate total weight (sum of spawn probabilities)
    float totalWeight = 0.0f;
    for (const auto* def : eligibleDefs) {
        totalWeight += def->spawnProbability;
    }
    
    // Perform weighted random selection.
    std::uniform_real_distribution<float> weightedDist(0.0f, totalWeight);
    float weightedRoll = weightedDist(rng);
    const SpawnDefinition* chosenDef = nullptr;
    for (const auto* def : eligibleDefs) {
        weightedRoll -= def->spawnProbability;
        if (weightedRoll <= 0.0f) {
            chosenDef = def;
            break;
        }
    }
    if (!chosenDef) {
        chosenDef = eligibleDefs.back();
    }
    
    // Determine group size for the selected spawn definition.
    std::uniform_int_distribution<int> groupDist(chosenDef->group.minSize, chosenDef->group.maxSize);
    int groupSize = groupDist(rng);
    
    std::cout << "Spawning " << groupSize << " of type ";
    switch (chosenDef->creatureType) {
        case CreatureType::Mob:
            std::cout << "Mob";
            break;
        case CreatureType::Boss:
            std::cout << "Boss";
            break;
        case CreatureType::Mutual:
            std::cout << "Mutual";
            break;
    }
    std::cout << " at (" << candidatePos.x << ", " << candidatePos.y << ")" << std::endl;
    
    // Create the group of entities.
    spawnCreatureGroup(*chosenDef, candidatePos, groupSize);
}

void SpawnSystem::spawnCreatureGroup(const SpawnDefinition &def, const vec2 &basePos, int groupSize) {
    std::uniform_real_distribution<float> offsetDist(-20.0f, 20.0f);
    for (int i = 0; i < groupSize; ++i) {
        vec2 offset = { offsetDist(rng), offsetDist(rng) };
        vec2 spawnPos = { basePos.x + offset.x, basePos.y + offset.y };

        switch (def.creatureType) {
            case CreatureType::Mob:
            case CreatureType::Mutual:
                createMob(registry, spawnPos, 50);
                break;
            case CreatureType::Boss:
                createBoss(registry, spawnPos);
                break;
            default:
                break;
        }
    }
}

void SpawnSystem::processDespawning() {
    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0) {
        debug_printf(DebugType::SPAWN, "No player entity found (despawning)\n");
        return;
    }
        
    auto playerEntity = *playerView.begin();
    auto& playerMotion = registry.get<Motion>(playerEntity);
    vec2 playerPos = playerMotion.position;
    
    auto mobView = registry.view<Mob, Motion>();
    for (auto entity : mobView) {
        auto& mobMotion = registry.get<Motion>(entity);
        float dx = mobMotion.position.x - playerPos.x;
        float dy = mobMotion.position.y - playerPos.y;
        float sqDistance = dx * dx + dy * dy;
        if (sqDistance > (DESPAWN_ZONE_RADIUS * DESPAWN_ZONE_RADIUS)) {
            // Despawn the mob.
	        debug_printf(DebugType::SPAWN, "Destroying entity (spawn sys)\n");
            registry.destroy(entity);
        }
    }
}