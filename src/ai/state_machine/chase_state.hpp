#pragma once

#include "ai_state.hpp"
#include <iostream>

struct ChaseStateConfig {
    float chaseSpeed = 100.0f;
    float attackRange = 50.f;
    float detectionRange = 400.f;
};

class ChaseState : public AIState {
public:
    ChaseState() : config() {}
    ChaseState(const ChaseStateConfig& config) : config(config) {}
    void onEnter(entt::registry& registry, entt::entity entity) override;
    void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override;
    void onExit(entt::registry& registry, entt::entity entity) override;

protected:
    ChaseStateConfig config;
};