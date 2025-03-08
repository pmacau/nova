#include "patrol_state.hpp"
#include "tinyECS/components.hpp"  // For Motion component.
#include <iostream>
#include <cmath>
#include <random>
#include "chase_state.hpp"
#include <ai/ai_component.hpp>
#include "attack_state.hpp"

PatrolState::PatrolState()
    : config(), patrolTarget({0.f, 0.f})
{
    std::random_device rd;
    rng.seed(rd());
}

PatrolState::PatrolState(const PatrolStateConfig& config)
    : config(config), patrolTarget({0.f, 0.f})
{
    std::random_device rd;
    rng.seed(rd());
}

void PatrolState::onEnter(entt::registry& registry, entt::entity entity) {
    std::cout << "Entering Patrol State\n";
    // When entering patrol, choose a new target within the patrol radius.
    auto& motion = registry.get<Motion>(entity);
    
    std::uniform_real_distribution<float> angleDist(0.0f, 2 * 3.14159265f);
    std::uniform_real_distribution<float> radiusDist(0.0f, config.patrolRadius);
    
    float angle = angleDist(rng);
    float radius = radiusDist(rng);
    
    vec2 offset = { radius * std::cos(angle), radius * std::sin(angle) };
    patrolTarget = motion.position + offset;
}

void PatrolState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    auto& motion = registry.get<Motion>(entity);

    // Check for player presence.
    auto playerView = registry.view<Player, Motion>();
    if (playerView.begin() != playerView.end()) {
        auto playerEntity = *playerView.begin();
        auto& playerMotion = registry.get<Motion>(playerEntity);
        vec2 diff = playerMotion.position - motion.position;
        float distanceToPlayer = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        // If within detection range, transition to ChaseState.
        if (distanceToPlayer < config.detectionRange) {
            static ChaseState chaseState;
            auto& aiComp = registry.get<AIComponent>(entity);
            aiComp.stateMachine->changeState(&chaseState);
            return;
        }

        // If even closer, transition to AttackState.
        if (distanceToPlayer < config.attackRange) {
            static AttackState attackState;
            auto& aiComp = registry.get<AIComponent>(entity);
            aiComp.stateMachine->changeState(&attackState);
            return;
        }
    }
    
    // Compute vector towards the patrol target.
    vec2 toTarget = patrolTarget - motion.position;
    float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
    
    if (distance < config.patrolThreshold) {
        // Target reached; choose a new target.
        std::uniform_real_distribution<float> angleDist(0.0f, 2 * 3.14159265f);
        std::uniform_real_distribution<float> radiusDist(0.0f, config.patrolRadius);
        float angle = angleDist(rng);
        float radius = radiusDist(rng);
        vec2 offset = { radius * std::cos(angle), radius * std::sin(angle) };
        patrolTarget = motion.position + offset;

        motion.velocity = {0, 0};
    } else {
        // Move toward the patrol target.
        vec2 direction = toTarget / distance;
        motion.velocity = direction * config.patrolSpeed;
    }
}

void PatrolState::onExit(entt::registry& registry, entt::entity entity) {
    std::cout << "Exiting Patrol State\n";
    auto& motion = registry.get<Motion>(entity);
    motion.velocity = {0.f, 0.f};
}