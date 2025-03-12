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
#include "quadtree/quadtree.hpp"

class CollisionSystem {
public:
    CollisionSystem(entt::registry& reg, WorldSystem& world, PhysicsSystem& physics);
    void step(float elapsed_ms);

    ~CollisionSystem() {
        if (quadTree) {
            quadTree->clear();
            delete quadTree;
        }
    }
    void initTree(int mapWidth, int mapHeight); //expensive
    
private:
    entt::registry& registry;
    PhysicsSystem& physics; 
    WorldSystem& world;
    QuadTree* quadTree;

    std::vector<entt::entity> destroy_entities;
    std::unordered_set<entt::entity> processed;

    void processHandler(entt::entity& e1, entt::entity& e2); 

    template<typename C1, typename C2>
    bool collision_type(entt::entity& e1, entt::entity& e2);

    template<typename T1, typename T2>
    void handle(entt::entity e1, entt::entity e2, float elapsed_ms);
    void resolve(entt::entity e1, entt::entity e2, float elapsed_ms);
};