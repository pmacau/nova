#pragma once

#include "creature_definition_data.hpp"
#include "creature/creature_common.hpp"   
#include "ai/ai_common.hpp"                
#include "ai/state_machine/transition.hpp" 
#include "animation/animation_definition.hpp"
#include "common.hpp"                     
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <iostream>
#include <creature/creature_defs/ai_defs/basic_fighter.hpp>
#include <creature/creature_defs/ai_defs/ai_globin_def.hpp>
#include <creature/creature_defs/blue_torch_goblin.hpp>
#include <animation/animation_manager.hpp>

class RedTorchGoblinDefData : public BlueTorchGoblinDefData {
public:
RedTorchGoblinDefData() {
        creatureID = CreatureID::GOBLIN_TORCH_RED;
        creatureType = CreatureType::Mob;
        initialize();
    }
    virtual ~RedTorchGoblinDefData() = default;

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

    static const RedTorchGoblinDefData& getInstance() {
        static RedTorchGoblinDefData instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        BlueTorchGoblinDefData::initializeSpawnInfo();
        spawnInfo.biomes = {Biome::B_SAVANNA};
    }
    
    virtual void initializeRenderingInfo() override {
        BlueTorchGoblinDefData::initializeRenderingInfo();

        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_RED;
    }
};
