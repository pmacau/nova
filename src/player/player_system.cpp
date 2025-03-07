#include "player_system.hpp"
#include "tinyECS/components.hpp"  // Contains Motion, Animation, Player, Sprite, etc.
#include "common.hpp"
#include <cmath>
#include <algorithm>

PlayerSystem::PlayerSystem(entt::registry& registry)
    : registry(registry)
{
}

void PlayerSystem::update(float deltaTime) {
    updatePlayerAnimationState();
}

void PlayerSystem::updatePlayerAnimationState() {
    // Assuming only one player entity.
    auto view = registry.view<Player, Motion, Animation, Sprite>();
    if (view.begin() == view.end())
        return;
    
    auto entity = *view.begin();
    auto& motion = registry.get<Motion>(entity);
    auto& anim = registry.get<Animation>(entity);
    auto& sprite = registry.get<Sprite>(entity);

    vec2 velocity = motion.velocity;
    
    // avoid toggling direction for slight noise
    constexpr float epsilon = 0.1f;
    
    // TODO: remove magic numbers row = 0, 1, 2 (can implement a state machine)
    if (std::abs(velocity.x) > epsilon || std::abs(velocity.y) > epsilon) {
        if (std::abs(velocity.x) > std::abs(velocity.y)) {
            anim.row = 1;
            // Flip horizontally if moving left.
            if (velocity.x < 0)
                motion.scale.x = -std::abs(motion.scale.x);
            else
                motion.scale.x = std::abs(motion.scale.x);
        }
        else {
            if (velocity.y > 0)
                anim.row = 0; // Up
            else
                anim.row = 2; // Down
            // Ensure no horizontal flip.
            motion.scale.x = std::abs(motion.scale.x);
        }
    }
    else {
        // Idle animation
        anim.row = 0;
    }
    // Do NOT update frame timer here.
    // The generic AnimationSystem is responsible for updating frameTime and currentFrameIndex.
}