#pragma once
#include "common.hpp"
#include <tinyECS/components.hpp>
#include <vector>
#include <string>

#include "animation_common.hpp"

struct SpriteSheet {
    TEXTURE_ASSET_ID textureAssetID;
    glm::vec2 sheetDimensions;
};

struct AnimationDefinition {
    std::string id;                     // Unique animation identifier (e.g., "idle", "walk", "attack")
    std::vector<FrameIndex> frames;        
    std::vector<float> frameDurations;  // Duration for each frame (ms). Can be a constant value too.
    bool loop;   
    float frameWidth;                   
    float frameHeight;
    
    SpriteSheet spriteSheet;

    bool flipForLeft = false;
};