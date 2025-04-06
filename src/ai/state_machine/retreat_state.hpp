#pragma once

#include <iostream>

#include "ai/ai_component.hpp"
#include "tinyECS/components.hpp"


class RetreatState : public AIState {
public:
    RetreatState() : stateComplete(false), currentWaypointIndex(0), pathRecalcTimer(0.0f) {
        id = "retreat";
    }
    virtual void onEnter(entt::registry& registry, entt::entity entity) override;
    virtual void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override;
    virtual void onExit(entt::registry& registry, entt::entity entity) override;
    
    virtual bool isStateComplete() const override { return stateComplete; }
    
private:
    std::vector<ivec2> retreatPath;
    int currentWaypointIndex = 0;
    float pathRecalcTimer = 0.0f;
    bool stateComplete = false;
    
    ivec2 computeRetreatDestination(entt::registry& registry, entt::entity entity, float retreatDistance);
    
    void regenerateRetreatPath(entt::registry& registry, entt::entity entity, ivec2 startTile, ivec2 targetTile);
};