// src/ai/state_machine/patrol_state.hpp
#pragma once

#include "ai_state.hpp"
#include <iostream>
#include <random>
#include <glm/vec2.hpp>


struct PatrolStateConfig {
    float patrolRadius = 100.0f;     
    float patrolSpeed = 25.0f;        
    float patrolThreshold = 5.0f;    
    float detectionRange = 400.0f;
    float attackRange = 50.0f;
};

class PatrolState : public AIState {
public:
    // Default constructor uses default config values.
    PatrolState();

    // Constructor that accepts a custom configuration.
    PatrolState(const PatrolStateConfig& config);

    virtual void onEnter(entt::registry& registry, entt::entity entity) override;
    virtual void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override;
    virtual void onExit(entt::registry& registry, entt::entity entity) override;

private:
    PatrolStateConfig config;
    glm::vec2 patrolTarget;
    std::default_random_engine rng;
};