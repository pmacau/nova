#include "enemy_definition.hpp"
#include <glm/glm.hpp>
#include <tinyECS/components.hpp>
#include <ai/ai_initializer.hpp>

// std::vector<CreatureDefinition> createEnemyDefinitions() {
//     std::vector<CreatureDefinition> enemyDefinitions;
//     {
//         CreatureDefinition def;

//         def.id = "goblin";
//         def.creatureType = CreatureType::Mob;

//         def.spawnInfo.spawnProbability = 0.5f;
//         def.spawnInfo.group.minSize = 1;
//         def.spawnInfo.group.maxSize = 2;
//         def.spawnInfo.biomes = {Biome::B_FOREST, Biome::B_BEACH, Biome::B_ICE, Biome::B_JUNGLE, Biome::B_SAVANNA};

//         // Basic stats
//         def.stats.minHealth = 50;
//         def.stats.maxHealth = 70;
//         def.stats.damage = 10;
//         def.stats.speed = 1.0f;

//         def.renderingInfo.scale = vec2(1344.f / 7, 960.f / 5) * 0.9f;
//         def.renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;
//         def.renderingInfo.spriteSheet.sheetDimensions = {1344.f, 960.f};
//         def.renderingInfo.animationMapping = {
//             {"idle", "mob2_idle"},
//             {"walk", "mob2_walk"},
//         };

//         // physics
//         def.physicsInfo.offset_to_ground = {0, def.renderingInfo.scale.y / 4.f * 0.8f};

//         float w = def.renderingInfo.scale.x * 0.4;
//         float h = def.renderingInfo.scale.y * 0.5;
//         def.physicsInfo.hitbox.pts = {
//             {w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
//             {w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
//         };

//         def.physicsInfo.hitbox.depth = 60;

//         // drops
//         def.dropInfo.dropItems = {"potion"};

//         // AI
//         def.aiInfo.aiConfig = getGoblinAIConfig();
//         def.aiInfo.transitionTable = getGoblinTransitionTable();

//         // Finally, push this definition into the global list
//         enemyDefinitions.push_back(def);
//     }

//     // for boss
//     {
//         CreatureDefinition def;
//         def.id = "boss";
//         def.creatureType = CreatureType::Boss;
//         def.spawnProbability = 1;
//         def.group.minSize = 1;
//         def.group.maxSize = 1;

//         def.biomes = {Biome::B_FOREST, Biome::B_BEACH, Biome::B_ICE, Biome::B_JUNGLE, Biome::B_SAVANNA};

//         def.minHealth = 100;
//         def.maxHealth = 100;
//         def.damage = 10;
//         def.speed = 1.0f;

//         def.scale = vec2(100, 120);
//         def.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;

//         def.offset_to_ground = {0, def.scale.y / 2.f};

//         enemyDefinitions.push_back(def);
//     }

//     return enemyDefinitions;
//     // ... define other enemies similarly ...
// }