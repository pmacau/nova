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
        if (registry.all_of<Projectile>(entity)) {
            auto& player_entity = *registry.view<Player>().begin();
            auto& player_motion = registry.get<Motion>(player_entity);
            if (abs(player_motion.position.x - motion.position.x) >= WINDOW_WIDTH_PX &&
                abs(player_motion.position.y - motion.position.y) >= WINDOW_HEIGHT_PX) {
                std::cout << "projectile destroyed\n";
                registry.destroy(entity);
            }
        }
    }
}
