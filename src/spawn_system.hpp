#pragma once
#include <random>
#include <entt.hpp>
#include "spawn_definitions.hpp"
#include <creature/boss_def.hpp>
#include <creature/creature_defs/creature_definition_data.hpp>

#include <spawn/spawn_common.hpp>

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

    void setSpawnRate(float rate) { spawnTimeInterval = rate; }
    void setSpawnCap(size_t cap) { spawnCap = cap; }

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
    std::vector<BossSpawn> bossSpawnData;

private:
    entt::registry& registry;
    std::default_random_engine rng;

    float spawnTimer = 0.0f;
    float spawnTimeInterval = 5000.0f;
    size_t spawnCap = 10;       // max mobs

    

    SpawnSystem(entt::registry& registry);
    ~SpawnSystem();


    void processNaturalSpawning();

    void processDespawning();

    void spawnCreaturesByTileIndices(const CreatureDefinitionData &def, const ivec2 &tileIndices, int groupSize);


    void checkAndSpawnBoss();

    void updateSpawnData() {
        auto playerView = registry.view<Player, Motion>();
        if (playerView.size_hint() == 0)
        {
            debug_printf(DebugType::SPAWN, "No player entity found (spawn)\n");
            return; // No player found.
        }
        auto playerEntity = *playerView.begin();
        auto &playerMotion = registry.get<Motion>(playerEntity);
        vec2 playerPos = playerMotion.position;

        // check biome
        auto biome = MapSystem::get_biome_by_indices(MapSystem::get_tile_indices(playerPos));
        auto it = biomeSpawnData.find(biome);
        if (it != biomeSpawnData.end()) {
            spawnCap = it->second.spawnCap;
            spawnTimeInterval = it->second.spawnTimeInterval;
        } else {
            spawnCap = 10; // default value
            spawnTimeInterval = 5000.0f; // default value
        }

        // check if it's night
        auto screen_entity = registry.view<ScreenState>().front();
        if (!registry.valid(screen_entity)) {
            debug_printf(DebugType::SPAWN, "Invalid screen entity\n");
            return;
        }
        auto& screen_state = registry.get<ScreenState>(screen_entity);

        if ((M_PI * 60.0) <= screen_state.time && screen_state.time <= (2.0 * M_PI * 60.0)) {
            spawnTimeInterval *= NIGHT_SPAWN_TIME_MULTIPLIER;
            spawnCap *= NIGHT_SPAWN_CAP_MULTIPLIER;
        }
    }

    void loadBossSpawnData();

    static SpawnSystem* instance;

};