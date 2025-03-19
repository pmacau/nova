#pragma once
#include "quad.hpp"


// Hoping that quad-tree can be usable not just in collisions if we plan on further refactoring other systems, as the screen since 
// it updates all entities that have motion within a certain distance of a player. 
// MOBS and PROJECTILES are not in the quadtree, as mobs despawn and we dont have many of them, and projectiles are short lived and also
// not many.
class QuadTree{
public:
    static constexpr int MAX_OBJECTS = 15; 
    static constexpr int MAX_LEVELS = 5;  

    QuadTree(float x, float y, float width, float height, int level = 0)
        : bounds(x, y, width, height), level(level) {
        children.fill(nullptr);
    }
    
    QuadTree* insert(entt::entity entity, const entt::registry& registry);
    std::vector<entt::entity> queryRange(const Quad& range, const entt::registry& registry);
    bool remove(entt::entity entity, const entt::registry& registry);
    void clear();
    Quad bounds;
    void initTree(entt::registry& registry);
    QuadTree* quadTree;
private:
    int level;
    std::vector<entt::entity> objects;
    std::array<QuadTree*, 4> children;

    void subdivide();
    QuadTree* insertIntoChildren(entt::entity entity, const entt::registry& registry);
};