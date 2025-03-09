#pragma once

#include <entt.hpp>

class AnimationSystem {
public:
    // Constructor accepts the ECS registry.
    AnimationSystem(entt::registry& registry) : registry(registry) {}

    // Call this each frame with the elapsed time (in milliseconds)
    void update(float deltaTime);

private:
    entt::registry& registry;
};
