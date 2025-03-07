#include "animation_manager.hpp"
#include "common.hpp"          
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
    for (int col = 0; col < 8; ++col) {
        walkDown.frames.push_back({3, col});
        walkDown.frameDurations.push_back(80.f);
    }
    animations[walkDown.id] = walkDown;


    // Mob
    AnimationDefinition mob2Idle;
    mob2Idle.id = "mob2_idle";
    mob2Idle.loop = true;
    mob2Idle.frameWidth = 1344.f / 7;
    mob2Idle.frameHeight = 960.f / 5;
    for (int col = 0; col < 7; ++col) {
        mob2Idle.frames.push_back({0, col});
        mob2Idle.frameDurations.push_back(150.f); // 150 ms per frame.
    }
    animations[mob2Idle.id] = mob2Idle;

    // --- Mob1 Walk Animation ---
    AnimationDefinition mob2Walk;
    mob2Walk.id = "mob2_walk";
    mob2Walk.loop = true;
    mob2Walk.frameWidth = 1344.f / 7;
    mob2Walk.frameHeight = 960.f / 5;
    for (int col = 0; col < 8; ++col) {
        mob2Walk.frames.push_back({1, col});
        mob2Walk.frameDurations.push_back(100.f); // 100 ms per frame.
    }
    animations[mob2Walk.id] = mob2Walk;


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
