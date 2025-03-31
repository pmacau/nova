#pragma once
#include <random>
#include <entt.hpp>
#include "spawn_definitions.hpp"  
#include <creature/enemy_definition.hpp>
#include <creature/creature_defs/creature_definition_data.hpp>

class SpawnSystem {
public:
    SpawnSystem(entt::registry& registry);
    ~SpawnSystem();

    void update(float deltaTime);

private:
    entt::registry& registry;
    std::default_random_engine rng;

    float spawnTimer = 0.0f;
    float spawnRate = 5000.0f;
    size_t spawnCap = 10;       // max mobs

    void processNaturalSpawning();

    void processDespawning();

    void spawnCreaturesByTileIndices(const CreatureDefinitionData &def, const ivec2 &tileIndices, int groupSize);


    void checkAndSpawnBoss();
};