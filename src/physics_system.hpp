#pragma once
#include "common.hpp"
//#include "collision_system.hpp"
#include <entt.hpp>
#include <cassert>
#include <sstream>
#include <iostream>
#include <tinyECS/components.hpp>

class CollisionSystem;

class PhysicsSystem {
public:
    PhysicsSystem(entt::registry& reg);
    void step(float elapsed_ms);
    void suppress(entt::entity& e1, entt::entity& e2);

private:
    entt::registry& registry;
    
    void checkMovement();
    void updateVelocity(float elapsed_s); 
    void stepAcceleration(float elapsed_s);
    vec2 getDirection(entt::entity e1, entt::entity e2); 

};