#pragma once
#include "common.hpp"
//#include "collision_system.hpp"
#include <entt.hpp>
#include <cassert>
#include <sstream>
#include <iostream>
#include <tinyECS/components.hpp>
#include "render_system.hpp"
#include "flag_system.hpp"

class CollisionSystem;
const float MAX_SPEED = 800.f;

class PhysicsSystem {
public:
    PhysicsSystem(entt::registry& reg, FlagSystem& flag_system);
    void step(float elapsed_ms);
    void suppress(entt::entity& e1, entt::entity& e2);
    void knockback(entt::entity& e1, entt::entity& e2, float force);
    void updatePlayerVelocity(InputState i);
    void block(Motion& motion);
    void dash(); 
private:
   
    entt::registry& registry;
    FlagSystem& flag_system;

    void updateVelocity(float elapsed_s); 
    void stepAcceleration(float elapsed_s);
    void updatePlayerState(float elapsed_s);
    vec2 getDirection(entt::entity e1, entt::entity e2); 

};