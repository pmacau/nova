#include "quadtree.hpp"


void QuadTree::insert(entt::entity entity, const Hitbox& hitbox) {
    if (children[0] != nullptr) {
        insertIntoChildren(entity, hitbox);
        return;
    }

    objects.push_back(entity);

    if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS) {
        if (children[0] == nullptr) {
            subdivide();
        }

        for (int i = 0; i < objects.size(); ++i) {
            insertIntoChildren(objects[i], hitbox);
        }
        objects.clear();
    }
}