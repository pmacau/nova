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
#include <creature/creature_defs/ai_defs/basic_ranger.hpp>
#include <creature/creature_defs/ai_defs/ai_globin_def.hpp>
#include <creature/creature_defs/small_blue_archer.hpp>
#include <animation/animation_manager.hpp>

class SmallYellowArcher : public SmallBlueArcher {
public:
SmallYellowArcher() {
        creatureID = CreatureID::SMALL_YELLOW_ARCHER;
        creatureType = CreatureType::Mob;
        initialize();
    }
    virtual ~SmallYellowArcher() = default;

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

    static const SmallYellowArcher& getInstance() {
        static SmallYellowArcher instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        SmallBlueArcher::initializeSpawnInfo();

        spawnInfo.biomes = {Biome::B_BEACH};
    }

    virtual void initializeRenderingInfo() override {
        SmallBlueArcher::initializeRenderingInfo();

        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::SMALL_YELLOW_ARCHER;
    }


    virtual void initializeAIInfo() override {
        SmallBlueArcher::initializeAIInfo();

        RangeAIConfig* config = static_cast<RangeAIConfig*>(aiInfo.aiConfig.get());

        config->shotsNumberRange = {1, 2}; // Number of shots to fire.
    }
};
