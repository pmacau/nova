#pragma once
#include "common.hpp"
#include "world_system.hpp"
#include <entt.hpp>

class CollisionSystem {
public:
    CollisionSystem(entt::registry& reg, WorldSystem& world);  
    void step(float elapsed_ms);

private:
    entt::registry& registry;
    WorldSystem& world;  
};