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
        renderingInfo.scale = { 43.f, 55.f };
        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::MOB;
        renderingInfo.spriteSheet.sheetDimensions = {43.f, 55.f};
    }

    virtual void initializePhysicsInfo() override {
        physicsInfo.scale = vec2(100, 120);

        physicsInfo.offset_to_ground = {0, physicsInfo.scale.y / 2.f};;
        float w = physicsInfo.scale.x;
	    float h = physicsInfo.scale.y;
        physicsInfo.hitbox.pts = {
            {w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
            {w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
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
        idle_right.loop = false; // a single image for now
        idle_right.frameWidth = frameWidth;
        idle_right.frameHeight = frameHeight;   
        idle_right.spriteSheet = renderingInfo.spriteSheet;

        idle_right.frames.push_back({0, 0});
        idle_right.frameDurations.push_back(1000.f);

        AnimationManager::getInstance().registerCreatureAnimation(creatureID, MotionAction::IDLE, MotionDirection::RIGHT, idle_right);

        
        // Create BlueTorch Goblin-specific walk animation.
        AnimationDefinition walk_right;
        walk_right.id = AnimationManager::getInstance().buildAnimationKey(animationHeader, MotionAction::WALK, MotionDirection::RIGHT);
        walk_right.loop = false;
        walk_right.frameWidth = frameWidth;
        walk_right.frameHeight = frameHeight;
        walk_right.spriteSheet = renderingInfo.spriteSheet;
        walk_right.frames.push_back({0, 0});
        walk_right.frameDurations.push_back(1000.f);
        AnimationManager::getInstance().registerCreatureAnimation(creatureID, MotionAction::WALK, MotionDirection::RIGHT, walk_right);

        renderingInfo.initAction = MotionAction::IDLE;
        renderingInfo.initDirection = MotionDirection::RIGHT;
        
        // Set up the animation mapping and default animation in rendering info.
        // renderingInfo.animationMapping.clear();
        // renderingInfo.animationMapping["idle"] = "blue_torch_goblin_idle";
        // renderingInfo.initialAnimationId = "idle";
    }

    virtual void initializeUIInfo() override {
        uiInfo.healthBar_y_adjust = - 10.f;
    }
};
