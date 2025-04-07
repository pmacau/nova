#include <iostream>
#include <cmath>
#include <algorithm>
#include "spawn_system.hpp"
#include <tinyECS/components.hpp>
#include <common.hpp>
#include <world_init.hpp>
#include <util/debug.hpp>
#include <map/map_system.hpp>
#include <map/tile.hpp>
#include <creature/creature_common.hpp>
#include <creature/creature_manager.hpp>

SpawnSystem* SpawnSystem::instance = nullptr;

void SpawnSystem::loadBossSpawnData() {
    bossSpawnData.clear();
    auto& bossIndices = MapSystem::getBossSpawnIndices();

    std::cout << "MapSystem boss indices count: " << bossIndices.size() << std::endl;
     
    for (auto& bossIndex : bossIndices) {

		if (MapSystem::get_biome_by_indices(bossIndex) == Biome::B_ICE) {
			bossSpawnData.push_back({ bossIndex, false, false, CreatureID::BOSS }); 
			//std::cout << "1Boss spawn: " << bossIndex.x << ", " << bossIndex.y << std::endl;
		}
		else if (MapSystem::get_biome_by_indices(bossIndex) == Biome::B_SAVANNA) {
			bossSpawnData.push_back({ bossIndex, false, false, CreatureID::BOSS_BEACH_RED }); 
			//std::cout << "2Boss spawn: " << bossIndex.x << ", " << bossIndex.y << std::endl;
		}
		else if (MapSystem::get_biome_by_indices(bossIndex) == Biome::B_JUNGLE) {
			bossSpawnData.push_back({ bossIndex, false, false, CreatureID::BOSS_FOREST_PURPLE }); 
			//std::cout << "3Boss spawn: " << bossIndex.x << ", " << bossIndex.y << std::endl;

		}
		else {
			bossSpawnData.push_back({ bossIndex, false, false, CreatureID::BOSS_JUNGLE_YELLOW });
			//std::cout << "4Boss spawn: " << bossIndex.x << ", " << bossIndex.y << std::endl;
		}
    }
}

SpawnSystem::SpawnSystem(entt::registry &registry)
    : registry(registry)
{
    loadBossSpawnData();
    debug_printf(DebugType::SPAWN, "SpawnSystem initialized.\n");

    // random seed.
    rng.seed(std::random_device()());
}

SpawnSystem::~SpawnSystem()
{
}

void SpawnSystem::update(float deltaTime)
{
    spawnTimer += deltaTime;

    checkAndSpawnBoss();

    // Check spawn cap
    size_t currentMobCount = registry.view<Mob>().size();
    if (currentMobCount >= spawnCap)
    {
        // std::cout << "Spawn cap reached (" << currentMobCount << " mobs). Skipping spawn." << std::endl;
    }
    else if (spawnTimer >= spawnTimeInterval)
    {
        spawnTimer = 0.0f;
        processNaturalSpawning();
    }

    processDespawning();
}

void SpawnSystem::processNaturalSpawning()
{
    // Get the player entity
    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0)
    {
        debug_printf(DebugType::SPAWN, "No player entity found (spawn)\n");
        return; // No player found.
    }
    auto playerEntity = *playerView.begin();
    auto &playerMotion = registry.get<Motion>(playerEntity);
    vec2 playerPos = playerMotion.position;

    // Compute rectangular boundaries of the spawn area
    vec2 halfSpawnZone = SPAWN_ZONE * 0.5f;
    vec2 spawnAreaWorldMin = playerPos - halfSpawnZone;
    vec2 spawnAreaWorldMax = playerPos + halfSpawnZone;

    // Compute the world-space safe area boundaries relative to the player
    vec2 halfSafeZone = SPAWN_SAFE_ZONE * 0.5f;
    vec2 safeAreaWorldMin = playerPos - halfSafeZone;
    vec2 safeAreaWorldMax = playerPos + halfSafeZone;

    // Convert the spawn area boundaries from world coordinates to tile indices
    int spawnTileMinX = std::max<int>(0, static_cast<int>(spawnAreaWorldMin.x / TILE_SIZE));
    int spawnTileMaxX = std::min<int>(MapSystem::map_width - 1, static_cast<int>(spawnAreaWorldMax.x / TILE_SIZE));
    int spawnTileMinY = std::max<int>(0, static_cast<int>(spawnAreaWorldMin.y / TILE_SIZE));
    int spawnTileMaxY = std::min<int>(MapSystem::map_height - 1, static_cast<int>(spawnAreaWorldMax.y / TILE_SIZE));

    // Convert the safe area boundaries from world coordinates to tile indices
    int safeTileMinX = std::max<int>(0, static_cast<int>(safeAreaWorldMin.x / TILE_SIZE));
    int safeTileMaxX = std::min<int>(MapSystem::map_width - 1, static_cast<int>(safeAreaWorldMax.x / TILE_SIZE));
    int safeTileMinY = std::max<int>(0, static_cast<int>(safeAreaWorldMin.y / TILE_SIZE));
    int safeTileMaxY = std::min<int>(MapSystem::map_height - 1, static_cast<int>(safeAreaWorldMax.y / TILE_SIZE));

    std::vector<ivec2> validTiles;

    // Iterate over tile indices within the spawn area.
    for (int tileY = spawnTileMinY; tileY <= spawnTileMaxY; ++tileY)
    {
        for (int tileX = spawnTileMinX; tileX <= spawnTileMaxX; ++tileX)
        {
            // Skip tiles that fall within the safe area.
            if (tileX >= safeTileMinX && tileX <= safeTileMaxX &&
                tileY >= safeTileMinY && tileY <= safeTileMaxY)
            {
                continue;
            }


            // Check if this tile is valid for spawning.
            Tile tileType = MapSystem::get_tile_type_by_indices(tileX, tileY);
            if (MapSystem::walkable_tile(tileType))
            {
                validTiles.push_back(ivec2(tileX, tileY));
            }
        }
    }

    if (validTiles.empty()) {
        debug_printf(DebugType::SPAWN, "(Warning) No valid spawnable tiles found in the spawn area.\n");
        return;
    }
    
    // Randomly pick a valid tile
    std::uniform_int_distribution<size_t> tileDist(0, validTiles.size() - 1);
    ivec2 candidate_tile_indices = validTiles[tileDist(rng)];

    // Assume candidate's biome is 0 for now
    // TODO: biome system
    Biome candidateBiome = MapSystem::get_biome_by_indices(candidate_tile_indices);

    // print the biome
    switch (candidateBiome)
    {
    case Biome::B_FOREST:
        debug_printf(DebugType::SPAWN, "Biome: Forest\n");
        break;
    case Biome::B_BEACH:
        debug_printf(DebugType::SPAWN, "Biome: Beach\n");
        break;
    case Biome::B_ICE:
        debug_printf(DebugType::SPAWN, "Biome: Ice\n");
        break;
    case Biome::B_JUNGLE:
        debug_printf(DebugType::SPAWN, "Biome: Jungle\n");
        break;
    case Biome::B_SAVANNA:
        debug_printf(DebugType::SPAWN, "Biome: Savanna\n");
        break;
    case Biome::B_OCEAN:
        debug_printf(DebugType::SPAWN, "Biome: Ocean\n");
        break;
    default:
        debug_printf(DebugType::SPAWN, "Biome: Unknown\n");
        break;
    }

    // Get all eligible spawn definitions for the given location
    std::vector<const CreatureDefinitionData *> eligibleDefs = CreatureManager::getInstance().queryDefinitions(
        [candidateBiome](const CreatureDefinitionData& def) {
            std::vector<Biome> defBiomes = def.getSpawnInfo().biomes;
            bool inBiome = std::find(defBiomes.begin(), defBiomes.end(), candidateBiome) != defBiomes.end();
            bool isMob = (def.getCreatureType() == CreatureType::Mob);
            return inBiome && isMob;
        });

    if (eligibleDefs.empty())
    {
        debug_printf(DebugType::SPAWN, "No eligible spawn definitions for biome \n");
        return;
    }

    // Calculate total weight (sum of spawn probabilities)
    float totalWeight = 0.0f;
    for (const auto *def : eligibleDefs)
    {
        totalWeight += def->getSpawnInfo().spawnProbability;
        // totalWeight += def->spawnProbability;
    }

    // Perform weighted random selection.
    std::uniform_real_distribution<float> weightedDist(0.0f, totalWeight);
    float weightedRoll = weightedDist(rng);
    const CreatureDefinitionData *chosenDef = nullptr;
    for (const auto *def : eligibleDefs)
    {
        weightedRoll -= def->getSpawnInfo().spawnProbability;
        if (weightedRoll <= 0.0f)
        {
            chosenDef = def;
            break;
        }
    }
    if (!chosenDef)
    {
        chosenDef = eligibleDefs.back();
    }

    // Determine group size for the selected spawn definition
    size_t currentMobCount = registry.view<Mob>().size();
    int availableSlots = spawnCap - static_cast<int>(currentMobCount);

    if (availableSlots < chosenDef->getSpawnInfo().group.minSize) {
        debug_printf(DebugType::SPAWN, "Not enough spawn slots available (%d available, need at least %d).\n", availableSlots, chosenDef->getSpawnInfo().group.minSize);
        return;
    }

    int minGroup = chosenDef->getSpawnInfo().group.minSize;
    int maxGroup = std::min(chosenDef->getSpawnInfo().group.maxSize, availableSlots);

    std::uniform_int_distribution<int> groupDist(minGroup, maxGroup);
    int groupSize = groupDist(rng);

    const char *creatureStr = "";
    switch (chosenDef->getCreatureType())
    {
    case CreatureType::Mob:
        creatureStr = "Mob";
        break;
    case CreatureType::Boss:
        creatureStr = "Boss";
        break;
    case CreatureType::Mutual:
        creatureStr = "Mutual";
        break;
    }
    debug_printf(DebugType::SPAWN, "Spawning %d of type %s at tile (%d, %d)\n", groupSize, creatureStr, (int) candidate_tile_indices.x, (int) candidate_tile_indices.y);
    // Create the group of entities.
    spawnCreaturesByTileIndices(*chosenDef, candidate_tile_indices, groupSize);
}


void SpawnSystem::spawnCreaturesByTileIndices(const CreatureDefinitionData &def, const ivec2 &tileIndices, int groupSize)
{
    std::vector<vec2> validNeighborTiles;

    int baseTileX = tileIndices.x;
    int baseTileY = tileIndices.y;

    // TODO: change later for customizable radius
    int spawnAreaMinX = std::max(0, baseTileX - 1);
    int spawnAreaMaxX = std::min(MapSystem::map_width - 1, baseTileX + 1);
    int spawnAreaMinY = std::max(0, baseTileY - 1);
    int spawnAreaMaxY = std::min(MapSystem::map_height - 1, baseTileY + 1);

    // Loop through neighbor tiles (include itself)
    for (int y = spawnAreaMinY; y <= spawnAreaMaxY; y++)
    {
        for (int x = spawnAreaMinX; x <= spawnAreaMaxX; x++)
        {
            vec2 neighborTileIndices = vec2(x, y);

            // world coordinate
            vec2 neighborCenter = MapSystem::get_tile_center_pos(neighborTileIndices);

            // Check if this tile is valid for spawning
            Tile tileType = MapSystem::get_tile(neighborCenter);
            if (MapSystem::walkable_tile(tileType))
            {
                validNeighborTiles.push_back(neighborCenter);
            }
        }
    }

    if (validNeighborTiles.empty())
    {
        debug_printf(DebugType::SPAWN, "[Error!!] No valid neighbor tiles for spawning\n");
    }

    std::uniform_int_distribution<size_t> tileIndexDist(0, validNeighborTiles.size() - 1);

    // TODO: remove magic number 1/4
    // offset distribution: using +/- TILE_SIZE/4 so that the offset is within the tile
    std::uniform_real_distribution<float> offsetWithinTile(-TILE_SIZE * 0.25f, TILE_SIZE * 0.25f);

    for (int i = 0; i < groupSize; ++i)
    {
        vec2 baseSpawnPos = validNeighborTiles[tileIndexDist(rng)];

        // Add a random offset within the tile
        vec2 offset = {offsetWithinTile(rng), offsetWithinTile(rng)};
        vec2 spawnFootPos = baseSpawnPos + offset;

        vec2 spawnPos = spawnFootPos - def.getPhysicsInfo().offset_to_ground;

        // Spawn the creature based on its type
        switch (def.getCreatureType())
        {
        case CreatureType::Mob:
        case CreatureType::Mutual:
            // createDebugTile(registry, MapSystem::get_tile_indices(spawnFootPos));
            // createDebugTile(registry, MapSystem::get_tile_indices(spawnPos));

            createCreature(registry, spawnPos, def, def.getStats().minHealth);

            // createMob2(registry, spawnPos, 50);
            break;
        case CreatureType::Boss:
            // createBoss(registry, spawnPos);
            break;
        default:
            break;
        }
    }
}

void SpawnSystem::processDespawning()
{
    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0)
    {
        debug_printf(DebugType::SPAWN, "No player entity found (despawning)\n");
        return;
    }

    auto playerEntity = *playerView.begin();
    auto &playerMotion = registry.get<Motion>(playerEntity);
    vec2 playerPos = playerMotion.position;

    vec2 halfDespawn = DESPAWN_ZONE * 0.5f;
    vec2 despawnAreaMin = playerPos - halfDespawn;
    vec2 despawnAreaMax = playerPos + halfDespawn;

    auto mobView = registry.view<Mob, Motion>();
    for (auto entity : mobView)
    {
        auto &mobMotion = registry.get<Motion>(entity);
        vec2 mobPos = mobMotion.position;

        if (mobPos.x < despawnAreaMin.x || mobPos.x > despawnAreaMax.x ||
            mobPos.y < despawnAreaMin.y || mobPos.y > despawnAreaMax.y)
        {
            debug_printf(DebugType::SPAWN, "Destroying entity at (%f, %f) (outside despawn zone)\n", mobPos.x, mobPos.y);

            // check if the entity is a boss
            for (auto& spawnData : bossSpawnData) {
                if (spawnData.entity == entity) {
                    spawnData.spawned = false;
                    spawnData.entity = entt::null;
                    break;
                }
            }

            destroy_creature(registry, entity);

        }
    }
}

void SpawnSystem::checkAndSpawnBoss() {
    // TODO: make this more robust

    // check if there is already a boss
    auto bossView = registry.view<Boss, Motion>();
    if (bossView.size_hint() > 0) {
        // debug_printf(DebugType::SPAWN, "Boss already exists\n");
        return;
    }

    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0) {
        debug_printf(DebugType::SPAWN, "No player entity found (boss check)\n");
        return;
    }
    auto playerEntity = *playerView.begin();
    auto& playerMotion = registry.get<Motion>(playerEntity);
    vec2 playerPos = playerMotion.position;

    // Compute the rectangular spawn area (using SPAWN_ZONE) centered on the player.
    vec2 halfSpawnZone = SPAWN_ZONE * 0.5f;
    vec2 spawnAreaMin = playerPos - halfSpawnZone;
    vec2 spawnAreaMax = playerPos + halfSpawnZone;

    // std::cout << "Length of bossSpawnData: " << bossSpawnData.size() << std::endl;
    for (auto& spawnData : bossSpawnData) {
        if (spawnData.spawned) {
            continue;
        }

        vec2 tileCenter = MapSystem::get_tile_center_pos(spawnData.spawnTile);
        if (tileCenter.x >= spawnAreaMin.x && tileCenter.x <= spawnAreaMax.x &&
            tileCenter.y >= spawnAreaMin.y && tileCenter.y <= spawnAreaMax.y) {

            const CreatureDefinitionData* def = CreatureManager::getInstance().getDefinition(spawnData.creatureID);

            // std::cout << "offset to ground: " << def->offset_to_ground.x << ", " << def->offset_to_ground.y << std::endl;
            vec2 spawnPos = tileCenter - def->getPhysicsInfo().offset_to_ground;

            // createDebugTile(registry, spawnData.spawnTile);
            // createDebugTile(registry, MapSystem::get_tile_indices(spawnPos));


            auto entity = createCreature(registry, spawnPos, *def, def->getStats().minHealth);


            debug_printf(DebugType::SPAWN, "Boss spawned at (%f, %f) from tile indices (%f, %f)\n", 
                         tileCenter.x, tileCenter.y, spawnData.spawnTile.x, spawnData.spawnTile.y);

            spawnData.spawned = true;
            spawnData.entity = entity;
            break;
        }
    }
}
