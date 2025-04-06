#pragma once
#include <string>
#include "animation_common.hpp"

struct AnimationComponent {
    float timer = 0.0f;             // Accumulated time.
    int currentFrameIndex = 0;      // Index of the current frame.

    std::string animation_header;
    MotionAction action;
    MotionDirection direction;

    bool flip = false;
    
    vec2 lastNormalizedVelocity = vec2(0.0f, 1.0f);
};