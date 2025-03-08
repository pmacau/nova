// #include "spawn_definitions.hpp"

// // global container for spawn definitions
// std::vector<SpawnDefinition> spawnDefinitions;

// void initializeSpawnDefinitions() {
//     spawnDefinitions.clear();

//     // Define a spawn definition for a basic Mob
//     SpawnDefinition mobDefinition;
//     mobDefinition.creatureType = CreatureType::Mob;
//     mobDefinition.spawnProbability = 0.5f; // 50% chance
//     mobDefinition.group.minSize = 1;
//     mobDefinition.group.maxSize = 3;
//     mobDefinition.texture = TEXTURE_ASSET_ID::MOB;
//     // can spawn in biomes 0 and 1
//     mobDefinition.biomeIDs = {0, 1};
//     spawnDefinitions.push_back(mobDefinition);

//     // Define a spawn definition for a mutual creature (e.g., normal animal)
//     SpawnDefinition animalDefinition;
//     animalDefinition.creatureType = CreatureType::Mutual;
//     animalDefinition.spawnProbability = 0.8f; // 80% chance
//     animalDefinition.group.minSize = 1;
//     animalDefinition.group.maxSize = 4;
//     animalDefinition.texture = TEXTURE_ASSET_ID::MOB;
//     // can spawn in biome 0 only
//     animalDefinition.biomeIDs = {0};
//     spawnDefinitions.push_back(animalDefinition);
// }