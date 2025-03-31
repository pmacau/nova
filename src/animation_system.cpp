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

        std::string animationId = AnimationManager::getInstance().buildAnimationKey(
            animComp.animation_header, animComp.action, animComp.direction);

        // const AnimationDefinition* animDef = AnimationManager::getInstance().getAnimation(animComp.currentAnimationId);
        const AnimationDefinition* animDef = AnimationManager::getInstance().getAnimation(animationId);
        if (!animDef) {
            std::cerr << "Animation definition not found for ID: " << animationId << "\n";
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