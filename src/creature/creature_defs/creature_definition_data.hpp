#pragma once

#include "creature/enemy_definition.hpp"
#include "ai/ai_common.hpp"              
#include "ai/state_machine/transition.hpp"
#include "animation/animation_definition.hpp" 
#include <unordered_map>
#include <string>

class CreatureDefinitionData {
public:
    CreatureDefinitionData();
    virtual ~CreatureDefinitionData() = default;

    const CreatureID& getCreatureID() const { return creatureID; }
    const CreatureType& getCreatureType() const { return creatureType; }
    const SpawnInfo& getSpawnInfo() const { return spawnInfo; }
    const Stats& getStats() const { return stats; }
    const RenderingInfo& getRenderingInfo() const { return renderingInfo; }
    const PhysicsInfo& getPhysicsInfo() const { return physicsInfo; }
    const DropInfo& getDropInfo() const { return dropInfo; }
    const AIInfo& getAIInfo() const { return aiInfo; }
    const UIInfo& getUIInfo() const { return uiInfo; }
    // const std::unordered_map<std::string, AnimationDefinition>& getAnimations() const { return animations; }

    virtual void initialize() {}

protected:
    // These methods are designed to be overridden by derived classes.
    // They provide creature-specific initialization.
    virtual void initializeSpawnInfo();
    virtual void initializeStats();
    virtual void initializeRenderingInfo();
    virtual void initializePhysicsInfo();
    virtual void initializeDropInfo();
    virtual void initializeAIInfo();
    virtual void initializeAnimations();
    virtual void initializeUIInfo();

    CreatureID creatureID;
    CreatureType creatureType;

    SpawnInfo spawnInfo;
    Stats stats;
    RenderingInfo renderingInfo;
    PhysicsInfo physicsInfo;
    DropInfo dropInfo;
    AIInfo aiInfo;
    UIInfo uiInfo;


private:
    CreatureDefinitionData(const CreatureDefinitionData&) = delete;
    CreatureDefinitionData& operator=(const CreatureDefinitionData&) = delete;
};