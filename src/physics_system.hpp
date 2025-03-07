#pragma once
#include "common.hpp"
//#include "collision_system.hpp"
#include <entt.hpp>
#include <cassert>
#include <sstream>
#include <iostream>
#include <tinyECS/components.hpp>
#include "render_system.hpp"


class CollisionSystem;

class PhysicsSystem {
public:
    PhysicsSystem(entt::registry& reg);
    void step(float elapsed_ms);
    void suppress(entt::entity& e1, entt::entity& e2);
    void knockback(entt::entity& e1, entt::entity& e2, float force);
    void ricochet(vec2& velocity, const vec2& normal);
    void updatePlayerVelocity(InputState i);
private:
    entt::registry& registry;
    void updateVelocity(float elapsed_s); 
    void stepAcceleration(float elapsed_s);
    vec2 getDirection(entt::entity e1, entt::entity e2); 
};