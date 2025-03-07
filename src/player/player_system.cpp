#include "player_system.hpp"
#include "tinyECS/components.hpp"  
#include "common.hpp"
#include <cmath>
#include <algorithm>
#include <animation/animation_component.hpp>

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
    auto& sprite = registry.get<Sprite>(entity);

    vec2 velocity = motion.velocity;
    constexpr float epsilon = 1.f;

    if (length(velocity) < epsilon) {
        // If velocity is very low, switch to idle animation.
        if (animComp.currentAnimationId != "player_idle") {
            animComp.currentAnimationId = "player_idle";
            animComp.timer = 0.0f;
            animComp.currentFrameIndex = 0;
        }
        // Ensure no horizontal flip.
        motion.scale.x = std::abs(motion.scale.x);
    }
    else {
        if (std::abs(velocity.x) > std::abs(velocity.y)) {
            animComp.currentAnimationId = "player_walk_right";
            // Flip sprite if moving left.
            if (velocity.x < 0)
                motion.scale.x = -std::abs(motion.scale.x);
            else
                motion.scale.x = std::abs(motion.scale.x);
        }
        else {
            if (velocity.y > 0)
                animComp.currentAnimationId = "player_walk_down";
            else
                animComp.currentAnimationId = "player_walk_up";
            // Ensure no horizontal flip.
            motion.scale.x = std::abs(motion.scale.x);
        }
    }
    // The generic AnimationSystem will read animComp.currentAnimationId and update frame timing.
}