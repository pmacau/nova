#include "animation_manager.hpp"
#include "common.hpp"          // For PLAYER_SPRITESHEET and vec2.
#include <iostream>
#include <tinyECS/components.hpp>

AnimationManager::AnimationManager() {
    initializeAnimations();
}

void AnimationManager::initializeAnimations() {
    animations.clear();

    // --- Idle Animation ---
    AnimationDefinition idle;
    idle.id = "player_idle";
    idle.loop = true;
    // Use the player's frame dimensions from PLAYER_SPRITESHEET.
    idle.frameWidth = PLAYER_SPRITESHEET.dims.x;
    idle.frameHeight = PLAYER_SPRITESHEET.dims.y;

    idle.frames.push_back({0, 0});
    idle.frameDurations.push_back(1000.f); 
    animations[idle.id] = idle;

    // --- Walk Right Animation ---
    AnimationDefinition walkRight;
    walkRight.id = "player_walk_right";
    walkRight.loop = true;
    walkRight.frameWidth = PLAYER_SPRITESHEET.dims.x;
    walkRight.frameHeight = PLAYER_SPRITESHEET.dims.y;
    // Assume walk right is on row 1 with 8 frames.
    for (int col = 0; col < 8; ++col) {
        walkRight.frames.push_back({1, col});
        walkRight.frameDurations.push_back(80.f); // 80 ms per frame.
    }
    animations[walkRight.id] = walkRight;

    // --- Walk Up Animation ---
    AnimationDefinition walkUp;
    walkUp.id = "player_walk_up";
    walkUp.loop = true;
    walkUp.frameWidth = PLAYER_SPRITESHEET.dims.x;
    walkUp.frameHeight = PLAYER_SPRITESHEET.dims.y;
    // Assume walk up is on row 2 with 8 frames.
    for (int col = 0; col < 8; ++col) {
        walkUp.frames.push_back({2, col});
        walkUp.frameDurations.push_back(80.f);
    }
    animations[walkUp.id] = walkUp;

    // --- Walk Down Animation ---
    AnimationDefinition walkDown;
    walkDown.id = "player_walk_down";
    walkDown.loop = true;
    walkDown.frameWidth = PLAYER_SPRITESHEET.dims.x;
    walkDown.frameHeight = PLAYER_SPRITESHEET.dims.y;
    // Assume walk down is on row 3 with 8 frames.
    for (int col = 0; col < 8; ++col) {
        walkDown.frames.push_back({3, col});
        walkDown.frameDurations.push_back(80.f);
    }
    animations[walkDown.id] = walkDown;

}

const AnimationDefinition* AnimationManager::getAnimation(const std::string& id) const {
    auto it = animations.find(id);
    if (it != animations.end()) {
        return &it->second;
    }
    std::cerr << "Animation not found for id: " << id << "\n";
    return nullptr;
}


AnimationManager g_animationManager;
