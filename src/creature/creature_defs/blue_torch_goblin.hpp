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
#include <creature/creature_defs/ai_defs/ai_globin_def.hpp>
#include <animation/animation_manager.hpp>

class BlueTorchGoblinDefData : public CreatureDefinitionData {
public:
    BlueTorchGoblinDefData() {
        creatureID = CreatureID::GOBLIN_TORCH_BLUE;
        creatureType = CreatureType::Mob;
        initialize();
    }
    virtual ~BlueTorchGoblinDefData() = default;

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

    static const BlueTorchGoblinDefData& getInstance() {
        static BlueTorchGoblinDefData instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        spawnInfo.spawnProbability = 0.6f;
        spawnInfo.group = {1, 2};
        spawnInfo.biomes = {Biome::B_ICE, Biome::B_FOREST};
    }

    virtual void initializeStats() override {
        stats.minHealth = 50;
        stats.maxHealth = 70;
        stats.damage = 10;
        stats.speed = 1.2f;
        creatureType = CreatureType::Mob; // Set the creature type.
    }

    virtual void initializeRenderingInfo() override {
        renderingInfo.scale = glm::vec2(1344.f / 7, 960.f / 5);
        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;
        renderingInfo.spriteSheet.sheetDimensions = glm::vec2(1344.f, 960.f);
    }

    virtual void initializePhysicsInfo() override {
        physicsInfo.scale = renderingInfo.scale  * 0.9f;
        physicsInfo.offset_to_ground = {0, renderingInfo.scale.y / 4.f * 0.8f};
        float w = renderingInfo.scale.x * 0.4f;
        float h = renderingInfo.scale.y * 0.5f;
        physicsInfo.hitbox.pts = {
            {w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
            {w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
        };
        physicsInfo.hitbox.depth = 60;
    }

    virtual void initializeDropInfo() override {
        dropInfo.dropItems.clear();

        DropItem potion;
        potion.type = Item::Type::POTION;
        potion.quantityRange.min = 1;
        potion.quantityRange.max = 3;
        potion.probability = 0.5f;
        dropInfo.dropItems.push_back(potion);

        DropItem iron;
        iron.type = Item::Type::IRON;
        iron.quantityRange.min = 1;
        iron.quantityRange.max = 5;
        iron.probability = 0.2f;
        dropInfo.dropItems.push_back(iron);

        DropItem copper;
        copper.type = Item::Type::COPPER;
        copper.quantityRange.min = 1;
        copper.quantityRange.max = 5;
        copper.probability = 0.3f;
        dropInfo.dropItems.push_back(copper);
    }

    virtual void initializeAIInfo() override {
        aiInfo.aiConfig = getGoblinAIConfig();
        aiInfo.transitionTable = &getBasicFighterTransitionTable();
        aiInfo.initialState = "patrol";
    }

    virtual void initializeAnimations() override {

        float frameWidth = renderingInfo.scale.x;
        float frameHeight = renderingInfo.scale.y;

        std::string animationHeader = AnimationManager::getInstance().creatureAnimationHeader(creatureID);
        // Create BlueTorch Goblin-specific idle animation.
        AnimationDefinition idle_right;
        idle_right.id = AnimationManager::getInstance().buildAnimationKey(animationHeader, MotionAction::IDLE, MotionDirection::RIGHT);
        idle_right.loop = true;
        idle_right.frameWidth = frameWidth;
        idle_right.frameHeight = frameHeight;   
        idle_right.spriteSheet = renderingInfo.spriteSheet;
        for (int col = 0; col < 7; ++col) {
            idle_right.frames.push_back({0, col});
            idle_right.frameDurations.push_back(150.f);
        }

        AnimationManager::getInstance().registerCreatureAnimation(creatureID, MotionAction::IDLE, MotionDirection::RIGHT, idle_right);
        
        // Create BlueTorch Goblin-specific walk animation.
        AnimationDefinition walk_right;
        walk_right.id = AnimationManager::getInstance().buildAnimationKey(animationHeader, MotionAction::WALK, MotionDirection::RIGHT);
        walk_right.loop = true;
        walk_right.frameWidth = frameWidth;
        walk_right.frameHeight = frameHeight;
        walk_right.spriteSheet = renderingInfo.spriteSheet;
        for (int col = 0; col < 6; ++col) {
            walk_right.frames.push_back({1, col});
            walk_right.frameDurations.push_back(100.f);
        }
        AnimationManager::getInstance().registerCreatureAnimation(creatureID, MotionAction::WALK, MotionDirection::RIGHT, walk_right);

        AnimationDefinition attack_right;
        attack_right.id = AnimationManager::getInstance().buildAnimationKey(animationHeader, MotionAction::ATTACK, MotionDirection::RIGHT);
        attack_right.loop = false;
        attack_right.frameWidth = frameWidth;
        attack_right.frameHeight = frameHeight;
        attack_right.spriteSheet = renderingInfo.spriteSheet;
        for (int col = 0; col < 6; ++col) {
            attack_right.frames.push_back({2, col});
            attack_right.frameDurations.push_back(100.f);
        }
        AnimationManager::getInstance().registerCreatureAnimation(creatureID, MotionAction::ATTACK, MotionDirection::RIGHT, attack_right);

        AnimationDefinition attack_down;
        attack_down.id = AnimationManager::getInstance().buildAnimationKey(animationHeader, MotionAction::ATTACK, MotionDirection::DOWN);
        attack_down.loop = false;
        attack_down.frameWidth = frameWidth;
        attack_down.frameHeight = frameHeight;
        attack_down.spriteSheet = renderingInfo.spriteSheet;
        for (int col = 0; col < 6; ++col) {
            attack_down.frames.push_back({3, col});
            attack_down.frameDurations.push_back(100.f);
        }
        AnimationManager::getInstance().registerCreatureAnimation(creatureID, MotionAction::ATTACK, MotionDirection::DOWN, attack_down);

        AnimationDefinition attack_up;
        attack_up.id = AnimationManager::getInstance().buildAnimationKey(animationHeader, MotionAction::ATTACK, MotionDirection::UP);
        attack_up.loop = false;
        attack_up.frameWidth = frameWidth;
        attack_up.frameHeight = frameHeight;
        attack_up.spriteSheet = renderingInfo.spriteSheet;
        for (int col = 0; col < 6; ++col) {
            attack_up.frames.push_back({4, col});
            attack_up.frameDurations.push_back(100.f);
        }
        AnimationManager::getInstance().registerCreatureAnimation(creatureID, MotionAction::ATTACK, MotionDirection::UP, attack_up);

        renderingInfo.initAction = MotionAction::IDLE;
        renderingInfo.initDirection = MotionDirection::RIGHT;
        
        // Set up the animation mapping and default animation in rendering info.
        renderingInfo.animationMapping.clear();
        renderingInfo.animationMapping["idle"] = "blue_torch_goblin_idle";
        renderingInfo.initialAnimationId = "idle";
    }

    virtual void initializeUIInfo() override {
        uiInfo.healthBar_y_adjust = physicsInfo.scale.y / 4.f;
    }
};
