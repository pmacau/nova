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
#include <creature/creature_defs/ai_defs/ai_purple_reaper_def.hpp>
#include <animation/animation_manager.hpp>


class BaseData : public CreatureDefinitionData {
public:
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

protected:
    virtual void initializeStats() override {
        stats.minHealth = 500;
        stats.maxHealth = 500;
        stats.damage = 10;
        stats.speed = 1.2f;
    }

    virtual void initializePhysicsInfo() override {
        physicsInfo.scale = vec2(35.f, 56.f) * 2.f;
        physicsInfo.offset_to_ground = { 0, physicsInfo.scale.y / 2.f };
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
        idle_right.loop = true;
        idle_right.frameWidth = frameWidth;
        idle_right.frameHeight = frameHeight;
        idle_right.spriteSheet = renderingInfo.spriteSheet;


        idle_right.frames.clear();
        idle_right.frameDurations.clear();


        idle_right.frames.push_back({ 0, 6 });
        idle_right.frames.push_back({ 0, 7 });
        idle_right.frames.push_back({ 0, 8 });
        idle_right.frames.push_back({ 0, 9 });

        
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
        uiInfo.healthBar_y_adjust = -10.f;
    }
};

// purple
class ForestPurpleWizardData : public BaseData {
public:
    ForestPurpleWizardData() {
        creatureID = CreatureID::BOSS_FOREST_PURPLE;
        creatureType = CreatureType::Boss;
        initialize();
    }

    static const ForestPurpleWizardData& getInstance() {
        static ForestPurpleWizardData instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        spawnInfo.spawnProbability = 0.6f;
        spawnInfo.group = { 1, 1 };
        spawnInfo.biomes = { Biome::B_JUNGLE };
    }

    virtual void initializeRenderingInfo() override {
        renderingInfo.scale = { 35.f, 56.f };
        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::MOB_PURPLE;
        renderingInfo.spriteSheet.sheetDimensions = { 350.f, 56.f };
    }
};

// red
class BeachPurpleWizardData : public BaseData {
public:
    BeachPurpleWizardData() {
        creatureID = CreatureID::BOSS;
        creatureType = CreatureType::Boss;
        initialize();
    }

    static const BeachPurpleWizardData& getInstance() {
        static BeachPurpleWizardData instance;
        return instance;
    }
    
protected:
    virtual void initializeSpawnInfo() override {
        spawnInfo.spawnProbability = 0.6f;
        spawnInfo.group = { 1, 1 };
        spawnInfo.biomes = { B_ICE };
    }

    virtual void initializeRenderingInfo() override {
        renderingInfo.scale = { 35.f, 56.f };
        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::MOB;
        renderingInfo.spriteSheet.sheetDimensions = { 350.f, 56.f };
    }
};

// yellow
class JunglePurpleWizardData : public BaseData {
public:
    JunglePurpleWizardData() {
        creatureID = CreatureID::BOSS_BEACH_RED;
        creatureType = CreatureType::Boss;
        initialize();
    }

    static const JunglePurpleWizardData& getInstance() {
        static JunglePurpleWizardData instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        spawnInfo.spawnProbability = 0.6f;
        spawnInfo.group = { 1, 1 };
        spawnInfo.biomes = { Biome::B_SAVANNA };
    }

    virtual void initializeRenderingInfo() override {
        renderingInfo.scale = { 35.f, 56.f };
        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::MOB_RED;
        renderingInfo.spriteSheet.sheetDimensions = { 350.f, 56.f };
    }
};

// default blue
class DefaultWizardData : public BaseData {
public:
    DefaultWizardData() {
        creatureID = CreatureID::BOSS_JUNGLE_YELLOW;
        creatureType = CreatureType::Boss;
        initialize();
    }

    static const DefaultWizardData& getInstance() {
        static DefaultWizardData instance;
        return instance;
    }

protected:
    virtual void initializeSpawnInfo() override {
        spawnInfo.spawnProbability = 0.6f;
        spawnInfo.group = { 1, 1 };
        spawnInfo.biomes = { Biome::B_BEACH}; 
    }

    virtual void initializeRenderingInfo() override {
        renderingInfo.scale = { 35.f, 56.f };
        renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::MOB_YELLOW;
        renderingInfo.spriteSheet.sheetDimensions = { 350.f, 56.f };
    }
};


