#pragma once
#include "common.hpp"
#include "world_system.hpp"
#include <entt.hpp>
#include <cassert>
#include <sstream>
#include <iostream>
#include "world_init.hpp"
#include "physics_system.hpp"


class CollisionSystem {
public:
    CollisionSystem(entt::registry& reg, WorldSystem& world, PhysicsSystem& physics);  

    static bool isContact(entt::entity e1, entt::entity e2, entt::registry& registry, float epsilon);

    void step(float elapsed_ms);

    std::set<entt::entity> collisionMarked; 


private:
    entt::registry& registry;
    PhysicsSystem& physics; 
    WorldSystem& world;  
};