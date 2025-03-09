// src/ai/state_machine/idle_state.hpp
#pragma once

#include "ai_state.hpp"

class IdleState : public AIState {
public:
    IdleState() {
        id = "idle";
    }
    // IdleState(const IdleStateConfig& config) : config(config) {}
    void onEnter(entt::registry& registry, entt::entity entity) override;

    void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override;

    void onExit(entt::registry& registry, entt::entity entity) override;
};