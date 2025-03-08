#pragma once
#include <random>
#include <entt.hpp>
#include "spawn_definitions.hpp"  
#include "ai/enemy_definition.hpp"


class SpawnSystem {
public:
    SpawnSystem(entt::registry& registry);
    ~SpawnSystem();

    void update(float deltaTime);

private:
    entt::registry& registry;
    std::default_random_engine rng;

    float spawnTimer = 0.0f;
    float spawnRate = 10000.0f;
    size_t spawnCap = 2;       // max mobs

    void processNaturalSpawning();

    void processDespawning();

    void spawnCreaturesByTileIndices(const EnemyDefinition &def, const vec2 &tileIndices, int groupSize);


    void checkAndSpawnBoss();
};