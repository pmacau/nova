#include "quad.hpp"


// Hoping that quad-tree can be usable not just in collisions if we plan on further refactoring other systems, as the screen since 
// it updates all entities that have motion within a certain distance of a player. 
class QuadTree{
    static constexpr int MAX_OBJECTS = 10; 
    static constexpr int MAX_LEVELS = 5;  

    QuadTree(float x, float y, float width, float height, int level = 0)
        : bounds(x, y, width, height), level(level) {
    }

    void insert(entt::entity entity, const Hitbox& hitbox);
    std::vector<entt::entity> queryRange(const Quad& range);
    void clear();

private:
    Quad bounds;
    int level;
    std::vector<entt::entity> objects;
    std::array<Quad*, 4> children;

    void subdivide();
    void insertIntoChildren(entt::entity entity, const Hitbox& hitbox);
};