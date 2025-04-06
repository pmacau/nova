#include "idle_state.hpp"
#include "ai/ai_component.hpp"
#include "ai/state_machine/chase_state.hpp"
#include "ai/ai_common.hpp" // for DETECTION_RANGE, etc.
#include "tinyECS/components.hpp"
#include <iostream>
#include <cmath>
#include <animation/animation_component.hpp>
#include <animation_system.hpp>

void IdleState::onEnter(entt::registry& registry, entt::entity entity) {
    // check if the entity has an Animation component
    if (registry.any_of<AnimationComponent>(entity)) {
        auto& animComp = registry.get<AnimationComponent>(entity);
        AnimationSystem::setAnimationAction(animComp, MotionAction::IDLE);
    }

}

void IdleState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {

}

void IdleState::onExit(entt::registry& registry, entt::entity entity) {
}