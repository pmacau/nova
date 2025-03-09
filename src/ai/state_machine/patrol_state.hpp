// src/ai/state_machine/patrol_state.hpp
#pragma once

#include "ai_state.hpp"
#include <iostream>
#include <random>
#include <glm/vec2.hpp>

class PatrolState : public AIState {
public:
PatrolState();
    virtual void onEnter(entt::registry& registry, entt::entity entity) override;
    virtual void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override;
    virtual void onExit(entt::registry& registry, entt::entity entity) override;

private:
    glm::vec2 patrolTarget;
    std::default_random_engine rng;
};