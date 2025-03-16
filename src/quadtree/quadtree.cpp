#include "quadtree.hpp"


void QuadTree::initTree(entt::registry& registry) {
    quadTree = new QuadTree(400.0f * 16.f, 400.0f * 16.f, 800 * 16.f, 800 * 16.f);
    auto playerEntity = registry.view<Player>().front();
    // quadTree->insert(playerEntity, registry);
    auto view = registry.view<Motion, Hitbox>(entt::exclude<Player, UIShip, Item, Title, TextData>); //currently reloads entire tree per frame
    for (auto entity : view) { 
        quadTree->insert(entity, registry);
    }


}

QuadTree* QuadTree::insert(entt::entity entity, const entt::registry& registry) {
    if (children[0] != nullptr) {
        return insertIntoChildren(entity, registry);
    }

    objects.push_back(entity);

    if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS) {
        if (children[0] == nullptr) {
            subdivide();
        }

        auto tempObjects = objects;
        objects.clear();
        for (auto entity : tempObjects) {
            insertIntoChildren(entity, registry);
        }
    }
    return this; 
}

void QuadTree::subdivide() {
    float subWidth = bounds.width / 2;
    float subHeight = bounds.height / 2;

    children[0] = new QuadTree(bounds.x - subWidth / 2, bounds.y - subHeight / 2, subWidth, subHeight, level + 1); // Top-left
    children[1] = new QuadTree(bounds.x + subWidth / 2, bounds.y - subHeight / 2, subWidth, subHeight, level + 1); // Top-right
    children[2] = new QuadTree(bounds.x - subWidth / 2, bounds.y + subHeight / 2, subWidth, subHeight, level + 1); // Bottom-left
    children[3] = new QuadTree(bounds.x + subWidth / 2, bounds.y + subHeight / 2, subWidth, subHeight, level + 1); // Bottom-right
}

QuadTree* QuadTree::insertIntoChildren(entt::entity entity, const entt::registry& registry) {
    const auto& motion = registry.get<Motion>(entity);

    // Create a quad from the hitbox
    Quad entityQuad(motion.position.x, motion.position.y, motion.scale.x, motion.scale.y);
    for (int i = 0; i < 4; ++i) {
        if (children[i]->bounds.intersects(entityQuad)) {
            return children[i]->insert(entity, registry);
        }
    }
    return nullptr;
}

bool QuadTree::remove(entt::entity entity, const entt::registry& registry) {
    // If this node has children, try to remove from them first
    if (children[0] != nullptr) {
        const auto& motion = registry.get<Motion>(entity);

        // Create a quad from the entity's position (using same approach as in insert)
        Quad entityQuad(motion.position.x, motion.position.y, motion.scale.x, motion.scale.y);
        //
        // Try removing from each child that might contain the entity
        for (int i = 0; i < 4; ++i) {
            if (children[i]->bounds.intersects(entityQuad)) {
                if (children[i]->remove(entity, registry)) {
                    return true;
                }
            }
        }
    }

    // If not found in children or no children, try to remove from this node's objects
    auto it = std::find(objects.begin(), objects.end(), entity);
    if (it != objects.end()) {
        objects.erase(it);
        return true;
    }

    return false; // Entity not found in this node or its children
}

std::vector<entt::entity> QuadTree::queryRange(const Quad& range, const entt::registry& registry) {
    std::vector<entt::entity> results;

    if (!bounds.intersects(range)) {
        return results;
    }
    // Add entities from this node
    for (const auto& entity : objects) {
        const auto& motion = registry.get<Motion>(entity);
        Quad entityQuad(motion.position.x, motion.position.y, motion.scale.x, motion.scale.y);

        if (range.intersects(entityQuad)) {
            results.push_back(entity);


        }
    }

    // Add entities from children
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



