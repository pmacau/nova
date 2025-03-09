#include "attack_state.hpp"
#include "ai/ai_component.hpp"
#include "ai/state_machine/idle_state.hpp"
#include "tinyECS/components.hpp"
#include <iostream>

void AttackState::onEnter(entt::registry& registry, entt::entity entity) {
    // debug_printf(DebugType::AI, "AttackState: onEnter\n");
}

void AttackState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    // debug_printf(DebugType::AI, "AttackState: onUpdate (deltaTime: %f)\n", deltaTime);
}

void AttackState::onExit(entt::registry& registry, entt::entity entity) {
    // debug_printf(DebugType::AI, "AttackState: onExit\n");
}