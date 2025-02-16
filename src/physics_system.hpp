#pragma once
#include "common.hpp"
#include "collision_system.hpp"
#include <entt.hpp>
#include <cassert>
#include <sstream>
#include <iostream>
#include <tinyECS/components.hpp>



class PhysicsSystem {
public:
    PhysicsSystem(entt::registry& reg, CollisionSystem& CollisionSystem);
    void step(float elapsed_ms);


private:
    entt::registry& registry;
    CollisionSystem& world;
    
    void checkMovement();

};