#pragma once
#include "common.hpp"
#include "world_system.hpp"
#include <entt.hpp>
#include <cassert>
#include <sstream>
#include <iostream>
#include "world_init.hpp"
#include "physics_system.hpp"
#include "collision/hitbox.hpp"

class CollisionSystem {
public:
    CollisionSystem(entt::registry& reg, WorldSystem& world, PhysicsSystem& physics);  

    //static bool isContact(entt::entity e1, entt::entity e2, entt::registry& registry, float epsilon);

    void step(float elapsed_ms);

    //std::unordered_map<entt::entity, glm::vec2> collisionMarked; 

private:
    entt::registry& registry;
    PhysicsSystem& physics; 
    WorldSystem& world;

    template<typename C1, typename C2>
    bool collision_type(entt::entity& e1, entt::entity& e2);

    template<typename T1, typename T2>
    void handle(entt::entity e1, entt::entity e2, float elapsed_ms);

    void resolve(entt::entity e1, entt::entity e2, float elapsed_ms);
    // collision helpers
    // void handleBlock(entt::entity e1, entt::entity e2, entt::registry& registry);
	// static vec2 getNormal(const Motion& m1, const HitBox& h1, const Motion& m2, const HitBox& h2);
    // static bool circlesCollision(const Motion& m1, const HitBox& h1, const Motion& m2, const HitBox& h2, float epsilon);
    // static bool circleRectCollision(const Motion& m1, const HitBox& h1, const Motion& m2, const HitBox& h2, float epsilon); 
	// static bool RectRectCollision(const Motion& m1, const HitBox& h1, const Motion& m2, const HitBox& h2, float epsilon);
    // static vec2 rectangleClamp(const Motion& rect_motion, const HitBox& rect_hitbox, const Motion& circle_motion, const HitBox& circle_hitbox);
};