#pragma once

#include "animation_definition.hpp" 
#include <unordered_map>
#include <string>
#include <unordered_map>
#include <creature/creature_common.hpp>

class AnimationManager {

public:
    static AnimationManager& getInstance();

    // Returns the animation definition associated with the given ID.
    const AnimationDefinition* getAnimation(CreatureID creatureId, const std::string& animationName) const;

private:
    AnimationManager();
    ~AnimationManager() = default;
    AnimationManager(const AnimationManager&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;

    // Initialize the animations with hardcoded data.
    // void initializeAnimations();

    // std::unordered_map<std::string, AnimationDefinition> animations;
};
