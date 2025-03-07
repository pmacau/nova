#pragma once
#include "common.hpp"
#include <vector>
#include <string>

struct FrameIndex {
    int row;
    int col;
};

// A definition for a single animation, loaded from external data.
struct AnimationDefinition {
    std::string id;                     // Unique animation identifier (e.g., "idle", "walk", "attack")
    std::vector<FrameIndex> frames;           // List of top-left coordinates of each frame in the spritesheet.
    std::vector<float> frameDurations;  // Duration for each frame (ms). Can be a constant value too.
    bool loop;   
    float frameWidth;                   // Width of a frame.
    float frameHeight;                       // Whether the animation should loop.
    // Optional: additional fields for transitions or events.
};