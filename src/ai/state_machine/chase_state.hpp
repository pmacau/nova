#pragma once

#include "ai_state.hpp"
#include <iostream>

class ChaseState : public AIState {
public:
    ChaseState() {
        id = "chase";
    }
    // ChaseState(const ChaseStateConfig& config) : config(config) {
    //     id = "chase";
    // }
    void onEnter(entt::registry& registry, entt::entity entity) override;
    void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override;
    void onExit(entt::registry& registry, entt::entity entity) override;
};