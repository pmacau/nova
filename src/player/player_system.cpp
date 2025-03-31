#include "player_system.hpp"
#include "tinyECS/components.hpp"  
#include "common.hpp"
#include <cmath>
#include <algorithm>
#include <animation/animation_component.hpp>
#include <animation/animation_manager.hpp>
#include <animation_system.hpp>

PlayerSystem::PlayerSystem(entt::registry& registry)
    : registry(registry)
{
}

void PlayerSystem::update(float deltaTime) {
    updatePlayerAnimationState();
}

void PlayerSystem::updatePlayerAnimationState() {
    auto view = registry.view<Player, Motion, AnimationComponent, Sprite>();
    if (view.begin() == view.end())
        return;
    
    auto entity = *view.begin();
    auto& motion = registry.get<Motion>(entity);
    auto& animComp = registry.get<AnimationComponent>(entity);
    //auto& sprite = registry.get<Sprite>(entity);

    vec2 velocity = motion.velocity;
    constexpr float epsilon = 1.f;

    std::string animation_id = AnimationManager::getInstance().buildAnimationKey(
        animComp.animation_header, animComp.action, animComp.direction);

    std::string player_walk_right = AnimationManager::getInstance().buildAnimationKey(AnimationManager::playerAnimationHeader(), MotionAction::WALK, MotionDirection::RIGHT);
    std::string player_walk_up = AnimationManager::getInstance().buildAnimationKey(AnimationManager::playerAnimationHeader(), MotionAction::WALK, MotionDirection::UP);
    std::string player_walk_down = AnimationManager::getInstance().buildAnimationKey(AnimationManager::playerAnimationHeader(), MotionAction::WALK, MotionDirection::DOWN);

    std::string player_idle_right = AnimationManager::getInstance().buildAnimationKey(AnimationManager::playerAnimationHeader(), MotionAction::IDLE, MotionDirection::RIGHT);
    std::string player_idle_up = AnimationManager::getInstance().buildAnimationKey(AnimationManager::playerAnimationHeader(), MotionAction::IDLE, MotionDirection::UP);
    std::string player_idle_down = AnimationManager::getInstance().buildAnimationKey(AnimationManager::playerAnimationHeader(), MotionAction::IDLE, MotionDirection::DOWN);

    if (length(velocity) < epsilon) {
        AnimationSystem::setAnimationAction(animComp, MotionAction::IDLE);
    }
    else {
        AnimationSystem::setAnimationAction(animComp, MotionAction::WALK);
    }
}