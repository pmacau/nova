#pragma once
#include "common.hpp"
#include <vector>
#include <string>

struct FrameIndex {
    int row;
    int col;
};

struct AnimationDefinition {
    std::string id;                     // Unique animation identifier (e.g., "idle", "walk", "attack")
    std::vector<FrameIndex> frames;        
    std::vector<float> frameDurations;  // Duration for each frame (ms). Can be a constant value too.
    bool loop;   
    float frameWidth;                   
    float frameHeight;                       
};