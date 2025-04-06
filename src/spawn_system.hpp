#pragma once
#include <random>
#include <entt.hpp>
#include "spawn_definitions.hpp"
#include <creature/boss_def.hpp>
#include <creature/creature_defs/creature_definition_data.hpp>

class SpawnSystem {
public:
static void initialize(entt::registry& registry) {
    if (!instance)
        instance = new SpawnSystem(registry);
    }

    static SpawnSystem& getInstance() {
        if (!instance) {
            throw std::runtime_error("SpawnSystem is not initialized! Call SpawnSystem::initialize(registry) first.");
        }
        return *instance;
    }


    void update(float deltaTime);

    void onRestartGame() {
        // Reset boss spawn data: if not defeated, set to not spawned.
        for (auto& spawnData : bossSpawnData) {
            if (!spawnData.defeated) {
                spawnData.spawned = false;
            }
        }
    }

    SpawnSystem(const SpawnSystem&) = delete;
    SpawnSystem& operator=(const SpawnSystem&) = delete;

private:
    entt::registry& registry;
    std::default_random_engine rng;

    float spawnTimer = 0.0f;
    float spawnRate = 5000.0f;
    size_t spawnCap = 10;       // max mobs

    std::vector<BossSpawn> bossSpawnData;

    SpawnSystem(entt::registry& registry);
    ~SpawnSystem();


    void processNaturalSpawning();

    void processDespawning();

    void spawnCreaturesByTileIndices(const CreatureDefinitionData &def, const ivec2 &tileIndices, int groupSize);


    void checkAndSpawnBoss();


    void loadBossSpawnData();

    static SpawnSystem* instance;

};