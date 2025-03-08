#pragma once

#include "animation_definition.hpp" 
#include <string>
#include <unordered_map>


class AnimationManager {
public:
    AnimationManager();
    ~AnimationManager() = default;

    const AnimationDefinition* getAnimation(const std::string& id) const;

private:
    std::unordered_map<std::string, AnimationDefinition> animations;

    // Initialize the animations with hardcoded data.
    void initializeAnimations();
};

extern AnimationManager g_animationManager;
