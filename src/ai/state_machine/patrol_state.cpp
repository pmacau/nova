#include "patrol_state.hpp"
#include "tinyECS/components.hpp"  // For Motion component.
#include <iostream>
#include <cmath>
#include <random>
#include "chase_state.hpp"
#include <ai/ai_component.hpp>
#include "attack_state.hpp"

PatrolState::PatrolState()
    : patrolTarget({0.f, 0.f})
{
    std::random_device rd;
    rng.seed(rd());
    id = "patrol";
}

void PatrolState::onEnter(entt::registry& registry, entt::entity entity) {
    // When entering patrol, choose a new target within the patrol radius.
    auto& motion = registry.get<Motion>(entity);

    auto& aiComp = registry.get<AIComponent>(entity);
    const AIConfig& config = aiComp.stateMachine->getConfig();
    
    std::uniform_real_distribution<float> angleDist(0.0f, 2 * 3.14159265f);
    std::uniform_real_distribution<float> radiusDist(0.0f, config.patrolRadius);
    
    float angle = angleDist(rng);
    float radius = radiusDist(rng);
    
    vec2 offset = { radius * std::cos(angle), radius * std::sin(angle) };
    patrolTarget = motion.position + offset;
}

void PatrolState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    auto& motion = registry.get<Motion>(entity);

    auto& aiComp = registry.get<AIComponent>(entity);
    const AIConfig& config = aiComp.stateMachine->getConfig();

    // Compute the vector toward the current patrol target.
    vec2 toTarget = patrolTarget - motion.position;
    float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

    if (distance < config.patrolThreshold) {
        // Target reached: choose a new patrol target.
        std::uniform_real_distribution<float> angleDist(0.0f, 2 * 3.14159265f);
        std::uniform_real_distribution<float> radiusDist(0.0f, config.patrolRadius);
        float angle = angleDist(rng);
        float radius = radiusDist(rng);
        vec2 offset = { radius * std::cos(angle), radius * std::sin(angle) };
        patrolTarget = motion.position + offset;
        
        // Optionally, stop movement briefly.
        motion.velocity = {0, 0};
    } else {
        // Move toward the patrol target.
        vec2 direction = toTarget / distance;
        motion.velocity = direction * config.patrolSpeed;
    }
}

void PatrolState::onExit(entt::registry& registry, entt::entity entity) {
    auto& motion = registry.get<Motion>(entity);
    motion.velocity = {0.f, 0.f};
}