#include "creature_definition_data.hpp"
#include "common.hpp"         
#include <glm/glm.hpp>
#include <iostream>


CreatureDefinitionData::CreatureDefinitionData() {
}

// // Returns the stored CreatureDefinition.
// const CreatureDefinition& CreatureDefinitionData::getDefinition() const {
//     return definition;
// }

// void CreatureDefinitionData::initialize() {
//     initializeCreatureDefinition();
//     initializeTransitionTable();
//     initializeAIConfig();
//     initializeAnimations();
// }

// // Default implementation for initializing the creature definition.
// // Derived classes should override this to provide creature‐specific values.
// void CreatureDefinitionData::initializeCreatureDefinition() {
//     // Basic identification.
//     definition.id = CreatureID::BASE_CREATURE;
//     definition.creatureType = CreatureType::Mob;
    
//     // Spawn Info defaults.
//     definition.spawnInfo.spawnProbability = 0.0f;
//     definition.spawnInfo.group = {0, 0};
//     definition.spawnInfo.biomes = {};  // Empty list.

//     // Gameplay Stats defaults.
//     definition.stats.minHealth = 0;
//     definition.stats.maxHealth = 0;
//     definition.stats.damage = 0;
//     definition.stats.speed = 0.0f;

    
//     // Drop Info defaults.
//     definition.dropInfo.dropItems.clear();
    
//     // Physics Info defaults.
//     definition.physicsInfo.offset_to_ground = {0, 0};
//     definition.physicsInfo.hitbox.pts.clear();
//     definition.physicsInfo.hitbox.depth = 0;
    
//     // AI Info defaults.
//     definition.aiInfo.aiConfig = AIConfig{};
//     definition.aiInfo.transitionTable = nullptr;
//     definition.aiInfo.initialState = "";
// }

// // Default implementation for initializing the transition table.
// // This creates a dummy transition table and assigns it to both the private pointer and the definition.
// void CreatureDefinitionData::initializeTransitionTable() {
//     static TransitionTable defaultTransitions;
//     if (defaultTransitions.empty()) {
//         // For example, a transition that never fires.
//         defaultTransitions["default"].push_back({
//             "default",
//             [](entt::registry&, entt::entity, const AIConfig&) -> bool { return false; }
//         });
//     }
//     transitionTable = &defaultTransitions;
//     definition.aiInfo.transitionTable = transitionTable;
//     definition.aiInfo.initialState = "default";
// }

// // Default implementation for initializing the AI configuration.
// void CreatureDefinitionData::initializeAIConfig() {
//     aiConfig.detectionRange = 300.0f;
//     aiConfig.unchaseRange = 500.0f;
//     aiConfig.chaseSpeed = 100.0f;
//     aiConfig.attackRange = 50.0f;
//     aiConfig.patrolRadius = 100.0f;
//     aiConfig.patrolSpeed = 25.f;
//     aiConfig.patrolThreshold = 5.0f;
//     definition.aiInfo.aiConfig = aiConfig;
// }


void CreatureDefinitionData::initializeSpawnInfo() {
    // Set a default CreatureID; derived classes should override.
    creatureID = CreatureID::BASE_CREATURE;
    creatureType = CreatureType::Mob;

    // Set default spawn info.
    spawnInfo.spawnProbability = 0.0f;
    spawnInfo.group = {0, 0};
    spawnInfo.biomes.clear();
}


void CreatureDefinitionData::initializeStats() {
    stats.minHealth = 0;
    stats.maxHealth = 0;
    stats.damage = 0;
    stats.speed = 0.0f;
}

void CreatureDefinitionData::initializeRenderingInfo() {
    // Set default rendering info.
    renderingInfo.scale = glm::vec2(0.0f, 0.0f);
    renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::PLAYER;
    renderingInfo.spriteSheet.sheetDimensions = glm::vec2(0.0f, 0.0f);
    // renderingInfo.animations.clear();
    renderingInfo.initialAnimationId = "";
}

void CreatureDefinitionData::initializePhysicsInfo() {
    physicsInfo.offset_to_ground = {0, 0};
    physicsInfo.hitbox.pts.clear();
    physicsInfo.hitbox.depth = 0;
}

void CreatureDefinitionData::initializeDropInfo() {
    dropInfo.dropItems.clear();
}

void CreatureDefinitionData::initializeAIInfo() {
    aiInfo.aiConfig = AIConfig{};
    aiInfo.transitionTable = nullptr;
    aiInfo.initialState = "";
}

void CreatureDefinitionData::initializeAnimations() {
    // Create a default idle animation as fallback.
    AnimationDefinition defaultAnim;
    defaultAnim.id = "default_idle";
    defaultAnim.loop = true;
    defaultAnim.frameWidth = 64.f;
    defaultAnim.frameHeight = 64.f;
    defaultAnim.spriteSheet = renderingInfo.spriteSheet; // Uses current sprite sheet.
    defaultAnim.frames.push_back({0, 0});
    defaultAnim.frameDurations.push_back(1000.f);
    
    // animations.clear();
    // animations["idle"] = defaultAnim;

    // Update the rendering info.
    // renderingInfo.
    renderingInfo.initialAnimationId = "idle";
}

// // Default implementation for initializing animations.
// // Here we create a single idle animation as a default.
// void CreatureDefinitionData::initializeAnimations() {
//     AnimationDefinition defaultAnim;
//     defaultAnim.id = "default_idle";
//     defaultAnim.loop = true;
//     defaultAnim.frameWidth = 64.f;
//     defaultAnim.frameHeight = 64.f;
//     // Assign the sprite sheet from rendering info.
//     defaultAnim.spriteSheet = renderingInfo.spriteSheet;
//     defaultAnim.frames.push_back({0, 0});
//     defaultAnim.frameDurations.push_back(1000.f);
    
//     animations.clear();
//     animations[defaultAnim.id] = defaultAnim;
    
//     // Rendering Info defaults.
//     definition.renderingInfo.scale = glm::vec2(0.0f, 0.0f);
//     // Use a dummy sprite sheet.
//     definition.renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::PLAYER;
//     definition.renderingInfo.spriteSheet.sheetDimensions = glm::vec2(0.0f, 0.0f);
//     definition.renderingInfo.animationMapping.clear();
//     definition.renderingInfo.animationMapping["idle"] = "default_idle";

//     definition.renderingInfo.initialAnimationId = "default_idle";
// }