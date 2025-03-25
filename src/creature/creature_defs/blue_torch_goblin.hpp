#pragma once

#include "creature_definition_data.hpp"
#include "creature/enemy_definition.hpp"
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

class BlueTorchGoblinDefData : public CreatureDefinitionData {
public:
    BlueTorchGoblinDefData() {
        creatureID = CreatureID::BLUE_TORCH_GOBLIN;
        creatureType = CreatureType::Mob;
        initialize();
    }
    virtual ~BlueTorchGoblinDefData() = default;

    // Explicit initialize method that calls all the initialization functions.
    virtual void initialize() override {
        // initializeCreatureDefinition();
        // initializeTransitionTable();
        // initializeAIConfig();
        // initializeAnimations();

        initializeSpawnInfo();
        initializeStats();
        initializeRenderingInfo();
        initializePhysicsInfo();
        initializeDropInfo();
        initializeAIInfo();
        initializeAnimations();
    }

    static const BlueTorchGoblinDefData& getInstance() {
        static BlueTorchGoblinDefData instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        spawnInfo.spawnProbability = 0.6f;
        spawnInfo.group = {1, 2};
        spawnInfo.biomes = {Biome::B_FOREST, Biome::B_BEACH};
    }

    virtual void initializeStats() override {
        stats.minHealth = 50;
        stats.maxHealth = 70;
        stats.damage = 10;
        stats.speed = 1.2f;
        creatureType = CreatureType::Mob; // Set the creature type.
    }

    virtual void initializeRenderingInfo() override {
        renderingInfo.scale = glm::vec2(1344.f / 7, 960.f / 5) * 0.9f;
        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;
        renderingInfo.spriteSheet.sheetDimensions = glm::vec2(1344.f, 960.f);
    }

    virtual void initializePhysicsInfo() override {
        renderingInfo.scale; // Already set in rendering info.
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
        dropInfo.dropItems = {"potion", "torch"};
    }

    virtual void initializeAIInfo() override {
        aiInfo.aiConfig = getGoblinAIConfig();
        aiInfo.transitionTable = &getBasicFighterTransitionTable();
        aiInfo.initialState = "patrol";
    }

    virtual void initializeAnimations() override {
        // Create BlueTorch Goblin-specific idle animation.
        AnimationDefinition idle;
        idle.id = "blue_torch_goblin_idle";
        idle.loop = true;
        idle.frameWidth = 1344.f / 7;
        idle.frameHeight = 960.f / 5;
        idle.spriteSheet = renderingInfo.spriteSheet;
        for (int col = 0; col < 7; ++col) {
            idle.frames.push_back({0, col});
            idle.frameDurations.push_back(150.f);
        }
        
        // Create BlueTorch Goblin-specific walk animation.
        AnimationDefinition walk;
        walk.id = "blue_torch_goblin_walk";
        walk.loop = true;
        walk.frameWidth = 1344.f / 7;
        walk.frameHeight = 960.f / 5;
        walk.spriteSheet = renderingInfo.spriteSheet;
        for (int col = 0; col < 8; ++col) {
            walk.frames.push_back({1, col});
            walk.frameDurations.push_back(100.f);
        }
        
        animations.clear();
        animations["idle"] = idle;
        animations["walk"] = walk;
        
        // Set up the animation mapping and default animation in rendering info.
        renderingInfo.animationMapping.clear();
        renderingInfo.animationMapping["idle"] = "blue_torch_goblin_idle";
        renderingInfo.initialAnimationId = "idle";
    }


    // Override the specific initialization functions for BlueTorch Goblin.
    // virtual void initializeCreatureDefinition() override {
    //     // Set basic info.
    //     definition.id = creatureID;
    //     definition.creatureType = CreatureType::Mob;
        
    //     // Spawn Info.
    //     definition.spawnInfo.spawnProbability = 0.6f;
    //     definition.spawnInfo.group = {1, 2};
    //     definition.spawnInfo.biomes = {Biome::B_FOREST, Biome::B_BEACH};
        
    //     // Gameplay Stats.
    //     definition.stats.minHealth = 50;
    //     definition.stats.maxHealth = 70;
    //     definition.stats.damage = 10;
    //     definition.stats.speed = 1.2f;
        
    //     // Rendering Info.
    //     definition.renderingInfo.scale = glm::vec2(1344.f / 7, 960.f / 5) * 0.9f;
    //     definition.renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;
    //     definition.renderingInfo.spriteSheet.sheetDimensions = glm::vec2(1344.f, 960.f);
        
    //     // Drop Info.
    //     definition.dropInfo.dropItems = {"potion", "torch"};
        
    //     // Physics Info.
    //     definition.physicsInfo.offset_to_ground = {0, definition.renderingInfo.scale.y / 4.f * 0.8f};
    //     float w = definition.renderingInfo.scale.x * 0.4f;
    //     float h = definition.renderingInfo.scale.y * 0.5f;
    //     definition.physicsInfo.hitbox.pts = {
    //         {w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
    //         {w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
    //     };
    //     definition.physicsInfo.hitbox.depth = 60;
    // }

    // virtual void initializeTransitionTable() override {
    //     // Use the goblin shared transition table.
    //     transitionTable = &getBasicFighterTransitionTable();
    //     definition.aiInfo.transitionTable = transitionTable;
    //     definition.aiInfo.initialState = "patrol";
    // }

    // virtual void initializeAIConfig() override {
    //     aiConfig = getGoblinAIConfig();
    //     definition.aiInfo.aiConfig = getGoblinAIConfig();
    // }

    // virtual void initializeAnimations() override {
    //     // Create BlueTorch Goblin-specific animations.
    //     AnimationDefinition idle;
    //     idle.id = "blue_torch_goblin_idle";
    //     idle.loop = true;
    //     idle.frameWidth = 1344.f / 7;
    //     idle.frameHeight = 960.f / 5;
    //     idle.spriteSheet = definition.renderingInfo.spriteSheet;
    //     for (int col = 0; col < 7; ++col) {
    //         idle.frames.push_back({0, col});
    //         idle.frameDurations.push_back(150.f);
    //     }
        
    //     AnimationDefinition walk;
    //     walk.id = "blue_torch_goblin_walk";
    //     walk.loop = true;
    //     walk.frameWidth = 1344.f / 7;
    //     walk.frameHeight = 960.f / 5;
    //     walk.spriteSheet = definition.renderingInfo.spriteSheet;
    //     for (int col = 0; col < 8; ++col) {
    //         walk.frames.push_back({1, col});
    //         walk.frameDurations.push_back(100.f);
    //     }
        
    //     animations.clear();
    //     animations["idle"] = idle;
    //     animations["walk"] = walk;


    //     definition.renderingInfo.animationMapping.clear();
    //     definition.renderingInfo.animationMapping["idle"] = "default_idle";

    //     definition.renderingInfo.initialAnimationId = "default_idle";
    // }
};

// Mark all inline definitions to prevent multiple definition issues.