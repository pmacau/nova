#pragma once

#include "ai_state.hpp"
#include <iostream>

class ChaseState : public AIState {
public:
    ChaseState() {
        id = "chase";
    }

    void onEnter(entt::registry& registry, entt::entity entity) override;
    void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override;
    void onExit(entt::registry& registry, entt::entity entity) override;

private:
    // Stores the computed path as a sequence of tile indices.
    std::vector<ivec2> currentPath;
    // Index of the next waypoint in the path.
    int currentWaypointIndex = 0;
    // Timer to control how often the path is recalculated.
    float pathRecalcTimer = 0.0f;
};