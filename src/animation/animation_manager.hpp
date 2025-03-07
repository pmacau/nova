#pragma once

#include "animation_definition.hpp" // Contains definition for AnimationDefinition.
#include <unordered_map>
#include <string>



class AnimationManager {
public:
    AnimationManager();
    ~AnimationManager() = default;

    // Retrieve an animation definition by its ID.
    const AnimationDefinition* getAnimation(const std::string& id) const;

private:
    // Hardcode animation definitions here.
    std::unordered_map<std::string, AnimationDefinition> animations;

    // Initialize the animations with hardcoded data.
    void initializeAnimations();
};

extern AnimationManager g_animationManager;
