#pragma once
#include <string>
#include "animation_common.hpp"

struct AnimationComponent {
    // std::string currentAnimationId; // ID of the current animation (e.g., "idle").
    float timer = 0.0f;             // Accumulated time.
    int currentFrameIndex = 0;      // Index of the current frame.

    std::string animation_header;
    MotionAction action;
    MotionDirection direction;
};