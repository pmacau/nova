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
    AnimationDefinition idle_down;
    idle_down.id = "player_idle_down";
    idle_down.loop = true;
    idle_down.frameWidth = PLAYER_SPRITESHEET.dims.x;
    idle_down.frameHeight = PLAYER_SPRITESHEET.dims.y;

    idle_down.frames.push_back({0, 0});
    idle_down.frameDurations.push_back(1000.f); 
    animations[idle_down.id] = idle_down;

    AnimationDefinition idle_up;
    idle_up.id = "player_idle_up";
    idle_up.loop = true;
    idle_up.frameWidth = PLAYER_SPRITESHEET.dims.x;
    idle_up.frameHeight = PLAYER_SPRITESHEET.dims.y;

    idle_up.frames.push_back({2, 0});
    idle_up.frameDurations.push_back(1000.f); 
    animations[idle_up.id] = idle_up;

    AnimationDefinition idle_right;
    idle_right.id = "player_idle_right";
    idle_right.loop = true;
    idle_right.frameWidth = PLAYER_SPRITESHEET.dims.x;
    idle_right.frameHeight = PLAYER_SPRITESHEET.dims.y;

    idle_right.frames.push_back({1, 0});
    idle_right.frameDurations.push_back(1000.f); 
    animations[idle_right.id] = idle_right;

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
