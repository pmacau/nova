#pragma once

#include <entt.hpp>

#include "common.hpp"            
#include "tinyECS/components.hpp"
#include "animation/animation_component.hpp"


class AnimationSystem {
public:
    // Constructor accepts the ECS registry.
    AnimationSystem(entt::registry& registry) : registry(registry) {}

    // Call this each frame with the elapsed time (in milliseconds)
    void update(float deltaTime);


    void updateAnimationDirection(const Motion& motion, AnimationComponent& animComp);

    static void setAnimationAction(AnimationComponent& animComp, MotionAction action) {
        if (animComp.action == action) return;

        animComp.action = action;
        animComp.currentFrameIndex = 0;
        animComp.timer = 0.0f;
    }

private:
    entt::registry& registry;
};
