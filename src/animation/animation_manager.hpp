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
    // const AnimationDefinition* getAnimation(CreatureID creatureId, const std::string& animationName) const;

    const AnimationDefinition* getAnimation(const std::string& id) const;

    const AnimationDefinition* getCreatureAnimation(CreatureID creatureId, MotionAction action, MotionDirection direction) const;

    void registerAnimation(const std::string& id, const AnimationDefinition& animDef);

    void registerCreatureAnimation(CreatureID creatureId, MotionAction action, MotionDirection direction, const AnimationDefinition& animDef);

    static std::string creatureAnimationHeader(CreatureID id) {
        return "creature_" + creatureIDToString(id);
    }

    static std::string playerAnimationHeader() {
        return "player";
    }

    static std::string creatureIDToString(CreatureID id) {
        return std::to_string(static_cast<int>(id));
    }

    static std::string buildAnimationKey(std::string header, MotionAction action, MotionDirection direction) {
        std::string actionStr = std::to_string(static_cast<int>(action));
        std::string directionStr = std::to_string(static_cast<int>(direction));
        return header + "_" + actionStr + "_" + directionStr;
    }

    static std::string buildCreatureAnimationKey(CreatureID creatureId, MotionAction action, MotionDirection direction) {
        std::string header = creatureAnimationHeader(creatureId);
        std::string actionStr = std::to_string(static_cast<int>(action));
        std::string directionStr = std::to_string(static_cast<int>(direction));
        return header + "_" + actionStr + "_" + directionStr;
    }


private:
    AnimationManager();
    ~AnimationManager() = default;
    AnimationManager(const AnimationManager&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;

    // Initialize the animations with hardcoded data.
    void initializeAnimations();

    std::unordered_map<std::string, AnimationDefinition> animations;
};
