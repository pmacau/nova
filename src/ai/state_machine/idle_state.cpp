#include "idle_state.hpp"
#include "ai/ai_component.hpp"
#include "ai/state_machine/chase_state.hpp"
#include "ai/ai_common.hpp" // for DETECTION_RANGE, etc.
#include "tinyECS/components.hpp"
#include <iostream>
#include <cmath>

void IdleState::onEnter(entt::registry& registry, entt::entity entity) {
}

void IdleState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {

}

void IdleState::onExit(entt::registry& registry, entt::entity entity) {
}