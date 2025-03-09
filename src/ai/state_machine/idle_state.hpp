// src/ai/state_machine/idle_state.hpp
#pragma once

#include "ai_state.hpp"

class IdleState : public AIState {
public:
    void onEnter(entt::registry& registry, entt::entity entity) override {
        // For example, set idle animation here.
    }

    void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override {
        // Idle behavior: look for targets or just remain idle.
    }

    void onExit(entt::registry& registry, entt::entity entity) override {
        // Cleanup if necessary.
    }
};