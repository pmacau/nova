#pragma once
#include "common.hpp"
#include "world_system.hpp"
#include <entt.hpp>
#include <cassert>
#include <sstream>
#include <iostream>


class CollisionSystem {
public:
    CollisionSystem(entt::registry& reg, WorldSystem& world);  

    static bool isContact(entt::entity e1, entt::entity e2, entt::registry& registry);

    void step(float elapsed_ms);

    std::set<entt::entity> collisionMarked; 

private:
    entt::registry& registry;
    WorldSystem& world;  


    void impossibleMovements();
};