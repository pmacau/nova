#include "chase_state.hpp"
#include "ai/ai_component.hpp"
#include "ai/state_machine/attack_state.hpp"
#include "ai/ai_common.hpp"  // for CHASE_SPEED, ATTACK_RANGE, etc.
#include "tinyECS/components.hpp"
#include <cmath>
#include <iostream>
#include "patrol_state.hpp"

void ChaseState::onEnter(entt::registry& registry, entt::entity entity) {
    std::cout << "ChaseState: onEnter\n";
    // Initialize chase-specific parameters if needed.
}

void ChaseState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    // Get player data.
    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0) return;
    
    auto playerEntity = *playerView.begin();
    auto& playerMotion = registry.get<Motion>(playerEntity);
    auto& motion = registry.get<Motion>(entity);

    auto& aiComp = registry.get<AIComponent>(entity);
    const AIConfig& config = aiComp.stateMachine->getConfig();
    
    // Compute direction vector toward the player.
    vec2 toPlayer = playerMotion.position - motion.position;
    float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

    if (dist > 0.0f) {
        vec2 direction = toPlayer / dist;
        // Use the chaseSpeed from the shared configuration stored in the state machine.
        motion.velocity = direction * config.chaseSpeed;
    }
}

void ChaseState::onExit(entt::registry& registry, entt::entity entity) {
    std::cout << "ChaseState: onExit\n";
    // Stop movement on exit.
    auto& motion = registry.get<Motion>(entity);
    motion.velocity = {0, 0};
}