#include "physics_system.hpp"
#include "ui_system.hpp"

PhysicsSystem::PhysicsSystem(entt::registry& reg, CollisionSystem& CollisionSystem):
	registry(reg),
	world(CollisionSystem)
{
}



void PhysicsSystem::checkMovement() {
    // Movement checking code here
}

void PhysicsSystem::step(float elapsed_ms) {
    float elapsed_s = elapsed_ms / 1000;
    auto players = registry.view<Motion>();
    for (auto entity : players) {
        auto& motion = registry.get<Motion>(entity);
        motion.position += motion.velocity * elapsed_s;
        if (registry.all_of<Player>(entity)) {
            UISystem::equipItem(registry, motion);
        }
    }
}
