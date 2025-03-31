#include "animation_system.hpp"
#include <algorithm>              
#include <iostream>
#include "animation/animation_definition.hpp"
#include "animation/animation_manager.hpp"

void AnimationSystem::update(float deltaTime) {
    // update animation direction
    auto view_dir = registry.view<Sprite, AnimationComponent, Motion>();
    for (auto entity : view_dir) {
        auto& motion = view_dir.get<Motion>(entity);
        auto& animComp = view_dir.get<AnimationComponent>(entity);
        updateAnimationDirection(motion, animComp);
    }


    // Iterate over all entities with both Sprite and Animation components.
    auto view = registry.view<Sprite, AnimationComponent, Motion>();
    for (auto entity : view) {
        auto& sprite = view.get<Sprite>(entity);
        auto& animComp = view.get<AnimationComponent>(entity);
        auto& motion   = view.get<Motion>(entity);

        std::string animationId = AnimationManager::getInstance().buildAnimationKey(
            animComp.animation_header, animComp.action, animComp.direction);

        // const AnimationDefinition* animDef = AnimationManager::getInstance().getAnimation(animComp.currentAnimationId);
        const AnimationDefinition* animDef = AnimationManager::getInstance().getAnimation(animationId);
        if (!animDef) {

            if (animComp.direction == MotionDirection::UP || animComp.direction == MotionDirection::DOWN) {
                // Set flip flag based on horizontal velocity and set canonical direction to RIGHT.
                vec2 velocity = motion.velocity.x == 0 ? animComp.lastNormalizedVelocity : motion.velocity;
                
                animComp.flip = (velocity.x < 0);
                animComp.direction = MotionDirection::RIGHT;
            }
            // Rebuild key after fallback.
            animationId = AnimationManager::getInstance().buildAnimationKey(
                animComp.animation_header, animComp.action, animComp.direction);
            animDef = AnimationManager::getInstance().getAnimation(animationId);

            if (!animDef) {
                std::cerr << "AnimationSystem: Animation definition not found for fallback key: " << animationId << "\n";
                continue;
            }
        }

        float currentFrameDuration = animDef->frameDurations[animComp.currentFrameIndex];
        
        animComp.timer += deltaTime;
        if (animComp.timer >= currentFrameDuration) {
            animComp.timer = 0.0f;
            animComp.currentFrameIndex++;
            // Wrap around if needed.
            if (animComp.currentFrameIndex >= static_cast<int>(animDef->frames.size())) {
                animComp.currentFrameIndex = animDef->loop ? 0 : (int)animDef->frames.size() - 1;
            }
        }
        
        sprite.coord = animDef->frames[animComp.currentFrameIndex];
        sprite.dims = vec2(animDef->frameWidth, animDef->frameHeight);

        // apply flip
        if (animComp.flip) {
            motion.scale.x = -std::abs(motion.scale.x);
        } else {
            motion.scale.x = std::abs(motion.scale.x);
        }
    }
}

void AnimationSystem::updateAnimationDirection(const Motion& motion, AnimationComponent& animComp) {
    vec2 velocity = motion.velocity;
    constexpr float epsilon = 1.f;

    vec velocity_normalize = normalize(velocity);

    if (length(velocity) >= epsilon) {
        animComp.lastNormalizedVelocity = velocity_normalize;

    } else {
        velocity_normalize = animComp.lastNormalizedVelocity;
    }
    
    // For horizontal dominance, we use RIGHT as canonical and update flip flag.
    if (std::abs(velocity_normalize.x) > std::abs(velocity_normalize.y)) {
        animComp.direction = MotionDirection::RIGHT;
        animComp.flip = (velocity_normalize.x < 0);
    } else {
        // For vertical dominance, use UP or DOWN based on the sign of velocity.y.
        animComp.direction = (velocity_normalize.y >= 0) ? MotionDirection::DOWN : MotionDirection::UP;
        animComp.flip = false;
    }
}