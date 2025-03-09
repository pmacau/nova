#include "attack_state.hpp"
#include "ai/ai_component.hpp"
#include "ai/state_machine/idle_state.hpp"
#include "tinyECS/components.hpp"
#include <iostream>

void AttackState::onEnter(entt::registry& registry, entt::entity entity) {
    std::cout << "AttackState: onEnter\n";
    // Start attack animation, reset attack timer, etc.
}

void AttackState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    std::cout << "AttackState: onUpdate (deltaTime: " << deltaTime << ")\n";
}

void AttackState::onExit(entt::registry& registry, entt::entity entity) {
    std::cout << "AttackState: onExit\n";
    // Cleanup attack state if needed.
}