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

class SmallRedArcher : public SmallBlueArcher {
public:
SmallRedArcher() {
        creatureID = CreatureID::SMALL_RED_ARCHER;
        creatureType = CreatureType::Mob;
        initialize();
    }
    virtual ~SmallRedArcher() = default;

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

    static const SmallRedArcher& getInstance() {
        static SmallRedArcher instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        SmallBlueArcher::initializeSpawnInfo();

        spawnInfo.biomes = {Biome::B_SAVANNA};
    }

    virtual void initializeRenderingInfo() override {
        SmallBlueArcher::initializeRenderingInfo();

        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::SMALL_RED_ARCHER;
    }


    virtual void initializeAIInfo() override {
        SmallBlueArcher::initializeAIInfo();

        RangeAIConfig* config = static_cast<RangeAIConfig*>(aiInfo.aiConfig.get());

        config->shotsNumberRange = {2, 4}; // Number of shots to fire.
    }
};
