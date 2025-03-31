#include "player_system.hpp"
#include "tinyECS/components.hpp"  
#include "common.hpp"
#include <cmath>
#include <algorithm>
#include <animation/animation_component.hpp>
#include <animation/animation_manager.hpp>

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
        // If velocity is very low, switch to idle animation.

        if (animation_id == player_walk_right) {
            animComp.action = MotionAction::IDLE;
            animComp.direction = MotionDirection::RIGHT;
            // animation_id = player_idle_right;
            animComp.timer = 0.0f;
            animComp.currentFrameIndex = 0;
        }
        else if (animation_id == player_walk_up) {
            // animation_id = player_idle_up;
            animComp.action = MotionAction::IDLE;
            animComp.direction = MotionDirection::UP;
            animComp.timer = 0.0f;
            animComp.currentFrameIndex = 0;
        }
        else if (animation_id == player_walk_down) {
            // animation_id = player_idle_down;
            animComp.action = MotionAction::IDLE;
            animComp.direction = MotionDirection::DOWN;
            animComp.timer = 0.0f;
            animComp.currentFrameIndex = 0;
        }
    }
    else {
        if (std::abs(velocity.x) > std::abs(velocity.y)) {
            // animation_id = player_walk_right;
            animComp.action = MotionAction::WALK;
            animComp.direction = MotionDirection::RIGHT;
            // Flip sprite if moving left.
            if (velocity.x < 0)
                motion.scale.x = -std::abs(motion.scale.x);
            else
                motion.scale.x = std::abs(motion.scale.x);
        }
        else {
            if (velocity.y > 0) {
                // animation_id = player_walk_down;
                animComp.action = MotionAction::WALK;
                animComp.direction = MotionDirection::DOWN;
            }
                
            else {
                // animation_id = player_walk_up;
                animComp.action = MotionAction::WALK;
                animComp.direction = MotionDirection::UP;
            }
            // Ensure no horizontal flip.
            motion.scale.x = std::abs(motion.scale.x);
        }
    }
    // The generic AnimationSystem will read animComp.currentAnimationId and update frame timing.
}