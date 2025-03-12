#include "quad.hpp"


// Hoping that quad-tree can be usable not just in collisions if we plan on further refactoring other systems, as the screen since 
// it updates all entities that have motion within a certain distance of a player. 
class QuadTree{
public:
    static constexpr int MAX_OBJECTS = 5; 
    static constexpr int MAX_LEVELS = 5;  

    QuadTree(float x, float y, float width, float height, int level = 0)
        : bounds(x, y, width, height), level(level) {
        children.fill(nullptr);
    }

    void insert(entt::entity entity, const entt::registry& registry);
    std::unordered_set<entt::entity> queryRange(const Quad& range, const entt::registry& registry);
    bool remove(entt::entity entity, const entt::registry& registry);
    void clear();

private:
    Quad bounds;
    int level;
    std::vector<entt::entity> objects;
    std::array<QuadTree*, 4> children;

    void subdivide();
    void insertIntoChildren(entt::entity entity, const entt::registry& registry);
};