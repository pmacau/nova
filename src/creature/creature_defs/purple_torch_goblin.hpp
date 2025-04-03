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
#include <creature/creature_defs/blue_torch_goblin.hpp>
#include <animation/animation_manager.hpp>

class PurpleTorchGoblinDefData : public BlueTorchGoblinDefData {
public:
PurpleTorchGoblinDefData() {
        creatureID = CreatureID::GOBLIN_TORCH_PURPLE;
        creatureType = CreatureType::Mob;
        initialize();
    }
    virtual ~PurpleTorchGoblinDefData() = default;

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

    static const PurpleTorchGoblinDefData& getInstance() {
        static PurpleTorchGoblinDefData instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        BlueTorchGoblinDefData::initializeSpawnInfo();
        spawnInfo.biomes = {Biome::B_JUNGLE};
    }
    
    virtual void initializeRenderingInfo() override {
        BlueTorchGoblinDefData::initializeRenderingInfo();

        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_PURPLE;
    }
};
