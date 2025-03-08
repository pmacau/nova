#include "idle_state.hpp"
#include "ai/ai_component.hpp"
#include "ai/state_machine/chase_state.hpp"
#include "ai/ai_common.hpp" // for DETECTION_RANGE, etc.
#include "tinyECS/components.hpp"
#include <iostream>
#include <cmath>

void IdleState::onEnter(entt::registry& registry, entt::entity entity) {
    std::cout << "IdleState: onEnter\n";
    // Reset or initialize any idle-specific data if needed.
}

void IdleState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    // Check if the player is within detection range.
    auto playerView = registry.view<Player, Motion>();
    if (playerView.begin() == playerView.end()) return;

    auto playerEntity = *playerView.begin();
    auto& playerMotion = registry.get<Motion>(playerEntity);
    auto& motion = registry.get<Motion>(entity);

    vec2 diff = playerMotion.position - motion.position;
    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    
    if (distance < config.detectionRange) {
        // Transition to ChaseState.
        // Use a static instance of ChaseState (for simplicity in this example).
        static ChaseState chaseState;
        auto& aiComp = registry.get<AIComponent>(entity);
        aiComp.stateMachine->changeState(&chaseState);
    }
}

void IdleState::onExit(entt::registry& registry, entt::entity entity) {
    std::cout << "IdleState: onExit\n";
    // Cleanup idle-specific state if needed.
}