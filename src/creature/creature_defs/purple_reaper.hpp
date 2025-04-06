#pragma once

#include "creature_definition_data.hpp"
#include "creature/creature_common.hpp"   // Contains enums like CreatureType, Biome, etc.
#include "ai/ai_common.hpp"                // For AIConfig defaults.
#include "ai/state_machine/transition.hpp" // For TransitionTable.
#include "animation/animation_definition.hpp"
#include "common.hpp"                      // For global asset definitions.
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <iostream>
#include <creature/creature_defs/ai_defs/basic_fighter.hpp>
#include <creature/creature_defs/ai_defs/ai_purple_reaper_def.hpp>
#include <animation/animation_manager.hpp>

class PurpleReaperData : public CreatureDefinitionData {
public:
    PurpleReaperData() {
        creatureID = CreatureID::BOSS;
        creatureType = CreatureType::Boss;
        initialize();
    }
    virtual ~PurpleReaperData() = default;

    // Explicit initialize method that calls all the initialization functions.
    virtual void initialize() override {

        initializeSpawnInfo();
        initializeStats();
        initializeRenderingInfo();
        initializePhysicsInfo();
        initializeDropInfo();
        initializeAIInfo();
        initializeAnimations();
        initializeUIInfo();

    }

    static const PurpleReaperData& getInstance() {
        static PurpleReaperData instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        spawnInfo.spawnProbability = 0.6f;
        spawnInfo.group = {1, 1};
        spawnInfo.biomes = {Biome::B_FOREST, Biome::B_BEACH};
    }

    virtual void initializeStats() override {
        stats.minHealth = 500;
        stats.maxHealth = 500;
        stats.damage = 10;
        stats.speed = 1.2f;
    }

    virtual void initializeRenderingInfo() override {
        renderingInfo.scale = { 80.f, 80.f };
        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::MOB;
        renderingInfo.spriteSheet.sheetDimensions = {240.f, 320.f};
    }

    virtual void initializePhysicsInfo() override {
        physicsInfo.scale = vec2(100, 100) * 1.5f;

        physicsInfo.offset_to_ground = {0, physicsInfo.scale.y / 2.f};;
        float w = physicsInfo.scale.x;
	    float h = physicsInfo.scale.y;
        physicsInfo.hitbox.pts = {
            {w * -0.5f * 0.5, h * -0.5f}, {w * 0.5f* 0.5, h * -0.5f},
            {w * 0.5f * 0.5, h * 0.5f},   {w * -0.5f * 0.5, h * 0.5f}
        };
        physicsInfo.hitbox.depth = 50;
    }

    virtual void initializeDropInfo() override {
        dropInfo.dropItems.clear();

        DropItem potion;
        potion.type = Item::Type::POTION;
        potion.quantityRange.min = 4;
        potion.quantityRange.max = 8;
        potion.probability = 0.5f;
        dropInfo.dropItems.push_back(potion);

        DropItem iron;
        iron.type = Item::Type::IRON;
        iron.quantityRange.min = 10;
        iron.quantityRange.max = 15;
        iron.probability = 1.f; // 100% chance to drop iron
        dropInfo.dropItems.push_back(iron);

        DropItem copper;
        copper.type = Item::Type::COPPER;
        copper.quantityRange.min = 4;
        copper.quantityRange.max = 8;
        copper.probability = 0.3f;
        dropInfo.dropItems.push_back(copper);
    }

    virtual void initializeAIInfo() override {
        aiInfo.aiConfig = getBossAIConfig();
        aiInfo.transitionTable = &getBasicFighterTransitionTable();
        aiInfo.initialState = "patrol";
    }



    virtual void initializeAnimations() override {
        float frameWidth = renderingInfo.scale.x;
        float frameHeight = renderingInfo.scale.y;

        std::string animationHeader = AnimationManager::getInstance().creatureAnimationHeader(creatureID);

       
        AnimationDefinition idle_right;
        idle_right.id = AnimationManager::getInstance().buildAnimationKey(animationHeader, MotionAction::IDLE, MotionDirection::RIGHT);
        idle_right.loop = true; 
        idle_right.frameWidth = frameWidth;
        idle_right.frameHeight = frameHeight;
        idle_right.spriteSheet = renderingInfo.spriteSheet;

       
        idle_right.frames.clear();
        idle_right.frameDurations.clear();

       
        idle_right.frames.push_back({ 2, 0 }); // Third row, second column
        idle_right.frames.push_back({ 2, 1 }); // Third row, third column
        idle_right.frames.push_back({ 2, 2 }); // Third row, fourth column
        idle_right.frames.push_back({ 3, 0 }); // Fourth row, first column

        // Set duration for each frame (in milliseconds)
        float frameDuration = 150.0f; 
        idle_right.frameDurations.push_back(frameDuration);
        idle_right.frameDurations.push_back(frameDuration);
        idle_right.frameDurations.push_back(frameDuration);
        idle_right.frameDurations.push_back(frameDuration);

        AnimationManager::getInstance().registerCreatureAnimation(creatureID, MotionAction::IDLE, MotionDirection::RIGHT, idle_right);

       
        AnimationDefinition walk_right = idle_right; 
        walk_right.id = AnimationManager::getInstance().buildAnimationKey(animationHeader, MotionAction::WALK, MotionDirection::RIGHT);
        AnimationManager::getInstance().registerCreatureAnimation(creatureID, MotionAction::WALK, MotionDirection::RIGHT, walk_right);

        renderingInfo.initAction = MotionAction::IDLE;
        renderingInfo.initDirection = MotionDirection::RIGHT;
    }

    virtual void initializeUIInfo() override {
        uiInfo.healthBar_y_adjust = - 10.f;
    }
};
