#include "animation_system.hpp"
#include "tinyECS/components.hpp" // Ensure this includes your Sprite and Animation definitions
#include "common.hpp"             // For any common definitions like TILE_SIZE, etc.
#include <algorithm>              // For std::min
#include <iostream>
#include "animation/animation_definition.hpp"
#include "animation/animation_manager.hpp"
#include "animation/animation_component.hpp"

void AnimationSystem::update(float deltaTime) {
    // Iterate over all entities with both Sprite and Animation components.
    auto view = registry.view<Sprite, AnimationComponent>();
    for (auto entity : view) {
        auto& sprite = view.get<Sprite>(entity);
        auto& animComp = view.get<AnimationComponent>(entity);

        const AnimationDefinition* animDef = g_animationManager.getAnimation(animComp.currentAnimationId);
        if (!animDef) {
            std::cerr << "Animation definition not found for ID: " << animComp.currentAnimationId << "\n";
            continue;
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
    }
}