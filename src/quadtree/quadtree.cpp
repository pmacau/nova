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
   /* float leftpoint_x = motion.position.x - (motion.scale.x) / 2;
    float leftpoint_y = motion.position.y - (motion.scale.y) / 2;*/
    Quad entityQuad(motion.position.x, motion.position.y, 400, 400);
    //
    for (int i = 0; i < 4; ++i) {
        if (children[i]->bounds.intersects(entityQuad)) {
            children[i]->insert(entity, registry);
        }
    }
}

bool QuadTree::remove(entt::entity entity, const entt::registry& registry) {
    // If this node has children, try to remove from them first
    if (children[0] != nullptr) {
        const auto& motion = registry.get<Motion>(entity);

        // Create a quad from the entity's position (using same approach as in insert)
        /*float leftpoint_x = motion.position.x - (motion.scale.x) / 2;
        float leftpoint_y = motion.position.y - (motion.scale.y) / 2;*/
        Quad entityQuad(motion.position.x, motion.position.y, 400, 400);
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

std::unordered_set<entt::entity> QuadTree::queryRange(const Quad& range, const entt::registry& registry) {
    std::unordered_set<entt::entity> results;

    if (!bounds.intersects(range)) {
        return results;
    }

    std::cout << "Range from player " << range.getX() << " " << range.getY() << std::endl;

    std::cout << "Set bounds from quad " << bounds.x << " " << bounds.y << std::endl;

   //  std::vector<entt::entity> toDelete; 
    // Add entities from this node
    for (const auto& entity : objects) {
        const auto& motion = registry.get<Motion>(entity);
       /* float leftpoint_x = motion.position.x - (motion.scale.x) / 2;
        float leftpoint_y = motion.position.y - (motion.scale.y) / 2;*/
        Quad entityQuad(motion.position.x, motion.position.y, 400, 400);

        if (range.intersects(entityQuad)) {
            results.insert(entity);
            //auto t = registry.get<RenderRequest>(entity); 
            /*if (t.used_texture == TEXTURE_ASSET_ID::PLAYER) {
                std::cout << "intersecting with player" << std::endl; 
            }
            else if (t.used_texture == TEXTURE_ASSET_ID::TREE){
                std::cout << "intersecting with tree" << std::endl;
            }
            else if (t.used_texture == TEXTURE_ASSET_ID::SHIP6) {
                std::cout << "intersecting with ship" << std::endl; 
            }
            else if (registry.view<Mob>().find(entity) != registry.view<Mob>().end()) {
                std::cout << "mobs" << std::endl; 
            }*/
            /*if (registry.view<Player>().find(entity) == registry.view<Player>().end()) {
                toDelete.push_back(entity); 
            }*/


        }
       /* for (auto entity : toDelete) {
            results.erase(entity);
        }
        const_cast<entt::registry&>(registry).destroy(toDelete.begin(), toDelete.end());*/
    }

    // Add entities from children
    if (children[0] != nullptr) {
        for (int i = 0; i < 4; ++i) {
            auto childResults = children[i]->queryRange(range, registry);
            results.insert(childResults.begin(), childResults.end());
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



