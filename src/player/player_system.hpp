#pragma once

#include <entt.hpp>

class PlayerSystem {
public:
    PlayerSystem(entt::registry& registry);

    // Update player-related logic (currently animation updates)
    // TODO:: move player logics (attack, use item, etc) here
    void update(float deltaTime);

private:
    entt::registry& registry;

    void updatePlayerAnimationState();
};