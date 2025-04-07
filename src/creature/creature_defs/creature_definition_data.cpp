#include "creature_definition_data.hpp"
#include "common.hpp"         
#include <glm/glm.hpp>
#include <iostream>


CreatureDefinitionData::CreatureDefinitionData() {
}

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

void CreatureDefinitionData::initializeUIInfo() {
    uiInfo.healthBar_y_adjust = 0.f;
}
