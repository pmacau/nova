#pragma once
#include <random>
#include <entt.hpp>
#include "spawn_definitions.hpp"  


class SpawnSystem {
public:
    SpawnSystem(entt::registry& registry);
    ~SpawnSystem();

    void update(float deltaTime);

private:
    entt::registry& registry;
    std::default_random_engine rng;

    float spawnTimer = 0.0f;
    float spawnRate = 2000.0f;
    size_t spawnCap = 10;       // max mobs

    void processSpawning();

    void processDespawning();

    void spawnCreaturesByTileIndices(const SpawnDefinition &def, const vec2 &tileIndices, int groupSize);


    void checkAndSpawnBoss();
};