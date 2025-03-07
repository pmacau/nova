#include "animation_system.hpp"
#include "tinyECS/components.hpp" // Ensure this includes your Sprite and Animation definitions
#include "common.hpp"             // For any common definitions like TILE_SIZE, etc.
#include <algorithm>              // For std::min
#include <iostream>

void AnimationSystem::update(float deltaTime) {
    // Iterate over all entities with both Sprite and Animation components.
    auto view = registry.view<Sprite, Animation>();
    for (auto entity : view) {
        auto& sprite = view.get<Sprite>(entity);
        auto& anim = view.get<Animation>(entity);

        anim.frameTime += deltaTime;
        if (anim.frameTime >= anim.frameDuration) {
            // Advance frame.
            anim.currentFrameIndex = (anim.currentFrameIndex + 1) % anim.totalFrames;
            anim.frameTime = 0.0f;
        }

        float frameWidth = anim.frameWidth;
        float frameHeight = anim.frameHeight;

        // Compute the top-left coordinate of the current frame in the spritesheet.
        // For instance, if the animation row is given by anim.row:
        // float frameX = anim.currentFrameIndex * frameWidth;
        // float frameY = anim.row * frameHeight;
        float frameX = anim.currentFrameIndex;
        float frameY = anim.row;
        
        sprite.coord = vec2(anim.row, anim.currentFrameIndex);
        sprite.dims = vec2(frameWidth, frameHeight);
    }
}