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

void QuadTree::subdivide() {
    float subWidth = bounds.width / 2;
    float subHeight = bounds.height / 2;

    children[0] = new QuadTree(bounds.x, bounds.y, subWidth, subHeight, level + 1);
    children[1] = new QuadTree(bounds.x + subWidth, bounds.y, subWidth, subHeight, level + 1);
    children[2] = new QuadTree(bounds.x, bounds.y + subHeight, subWidth, subHeight, level + 1);
    children[3] = new QuadTree(bounds.x + subWidth, bounds.y + subHeight, subWidth, subHeight, level + 1);
}

void QuadTree::insertIntoChildren(entt::entity entity, const entt::registry& registry) {
    const auto& motion = registry.get<Motion>(entity);

    // Create a quad from the hitbox
    Quad entityQuad(motion.position.x, motion.position.y, 400, 400); //THIS IS ASSUMING THE BIGGEST HITBOX IS A 50 X 50, not sure how we'd do this for all convex shapes

    for (int i = 0; i < 4; ++i) {
        if (children[i]->bounds.intersects(entityQuad)) {
            children[i]->insert(entity, registry);
        }
    }
}

std::vector<entt::entity> QuadTree::queryRange(const Quad& range, const entt::registry& registry) {
    std::vector<entt::entity> results;
    std::cout << "Query Range: (" << range.x << "," << range.y << ","
        << range.width << "," << range.height << ")" << std::endl;
    std::cout << "Tree Bounds: (" << bounds.x << "," << bounds.y << ","
        << bounds.width << "," << bounds.height << ")" << std::endl;

    if (!bounds.intersects(range)) {
        std::cout << "wooo" << std::endl; 
        return results;
    }

    for (const auto& entity : objects) {
        const auto& motion = registry.get<Motion>(entity);

        // Create a quad from the hitbox
        Quad entityQuad(motion.position.x, motion.position.y, 400, 400);

        if (range.intersects(entityQuad)) {
            results.push_back(entity);
        }
    }

    if (children[0] != nullptr) {
        for (int i = 0; i < 4; ++i) {
            auto childResults = children[i]->queryRange(range, registry);
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



