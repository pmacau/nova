#include "quadtree.hpp"




void QuadTree::insert(entt::entity entity, const entt::registry& registry) {
    if (children[0] != nullptr) {
        insertIntoChildren(entity, registry);
        return;
    }

    objects.push_back(entity);

    if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS) {
        if (children[0] == nullptr) {
            subdivide();
        }

        for (int i = 0; i < objects.size(); ++i) {
            insertIntoChildren(objects[i], registry);
        }
        objects.clear();
    }
}

void QuadTree::insertIntoChildren(entt::entity entity, const entt::registry& registry) {
    for (int i = 0; i < 4; ++i) {
        if (children[i]->bounds.intersects(Quad(hitbox.pts[0].x, hitbox.pts[0].y, hitbox.width, hitbox.height))) {
            children[i]->insert(entity, hitbox);
        }
    }
}

std::vector<entt::entity> QuadTree::queryRange(const Quad& range) {
    std::vector<entt::entity> results;

    if (!bounds.intersects(range)) {
        return results;
    }

    for (const auto& entity : objects) {
        const auto& hitbox = registry.get<Hitbox>(entity);
        if (range.intersects(Quad(hitbox.pts[0].x, hitbox.pts[0].y, hitbox.width, hitbox.height))) {
            results.push_back(entity);
        }
    }

    if (children[0] != nullptr) {
        for (int i = 0; i < 4; ++i) {
            auto childResults = children[i]->queryRange(range);
            results.insert(results.end(), childResults.begin(), childResults.end());
        }
    }

    return results;
}


void QuadTree::clear() {
    objects.clear();

    for (int i = 0; i < 4; ++i) {
        if (children[i] != nullptr) {
            children[i]->clear();
            delete children[i];
            children[i] = nullptr;
        }
    }
}



