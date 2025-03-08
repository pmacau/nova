// #pragma once

// #include <vector>
// #include <string>
// #include "common.hpp" 
// #include "tinyECS/components.hpp"
// #include "ai/ai_common.hpp"

// // Structure to group properties related to group spawning
// struct GroupProperties {
//     int minSize; // Minimum number in a group
//     int maxSize; // Maximum number in a group
// };

// // Spawn parameters for a creature
// struct SpawnDefinition {
//     CreatureType creatureType;    
//     float spawnProbability;       // Spawn probability (range 0.0 to 1.0).
//     GroupProperties group;        // Group spawn settings.
//     TEXTURE_ASSET_ID texture;     // Texture/asset reference for the creature.
//     std::vector<int> biomeIDs;    // List of biome IDs where this creature can spawn.
// };

// // Global container for spawn definitions.
// extern std::vector<SpawnDefinition> spawnDefinitions;

// void initializeSpawnDefinitions();