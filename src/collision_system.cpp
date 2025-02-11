#include "collision_system.hpp"
#include "world_init.hpp"

CollisionSystem::CollisionSystem(entt::registry& reg, WorldSystem& world) :
	registry(reg),
	world(world)
{
}

void CollisionSystem::impossibleMovements() {

}

void CollisionSystem::updateHealthbar(int health) {
	std::cout << health << std::endl;
	for (auto entity : registry.view<Healthbar>()) {
		auto& healthbar_motion = registry.get<Motion>(entity);
		float oldWidth = healthbar_motion.scale.x;
		healthbar_motion.scale = vec2({ health * (200.f / PLAYER_HEALTH), 15 });
		healthbar_motion.position.x += ((healthbar_motion.scale.x - oldWidth) / 2); // to keep the healthbar fixed to left because scaling happens relative to center
		break; // since there is only one healthbar, is there a better way instead of doing the loop?
	}
}


void CollisionSystem::step(float elapsed_ms)
{
	// TODO: likely refactor this if our collision system becomes more complicated which it will if we decide we want obstacles to not be considered entities 
	// between entities, also doesn't include projectiles yet. Also maybe implement a k-d tree to detect valid candidates.
	
	float elapsed_s = elapsed_ms / 1000.f;
	// All mobs
	auto mobs = registry.view<Mob>();
	// mob refresh
	for (auto mob : mobs) {
		auto& mob_ref = registry.get<Mob>(mob);
		if (mob_ref.hit_time > 0) {
			mob_ref.hit_time -= elapsed_s;
		}
	}
	auto entities = registry.view<Motion>();
	auto playerCheck = registry.view<Player>();
	for (auto entity : entities) { // does a for loop for projectiles and what not right, now this isn't necessary. 
		// only check once so decide if checks on player or invader (player chosen)
		// checks if ID is player
		if ((uint32_t)entt::entt_traits<entt::entity>::to_entity(playerCheck.front()) == (uint32_t)entt::entt_traits<entt::entity>::to_entity(entity)) {
			for (auto mob : mobs) {
				//std::cout << "ENTERED" << std::endl;
				if (isContact(mob, entity, registry, 40)){
					
					auto& player_ref = registry.get<Player>(entity);
					auto& mob_ref = registry.get<Mob>(mob);
					if (mob_ref.hit_time <= 0) {
						std::cout << "COLLISION" << std::endl;
						player_ref.health -= MOB_DAMAGE;
						updateHealthbar(player_ref.health);
						if (player_ref.health <= 0) {
							updateHealthbar(PLAYER_HEALTH);
							world.player_respawn();
						}
						mob_ref.hit_time = 1.f;
					}

				}


			}
		}

	}
}


// determines if tw
bool CollisionSystem::isContact(entt::entity e1 , entt::entity e2, entt::registry& registry, float epsilon) {
	Motion m1 = registry.get<Motion>(e1); 
	Motion m2 = registry.get<Motion>(e2);
	bool xCheck = m1.position.x < m2.position.x + epsilon && m1.position.x > m2.position.x - epsilon;
	bool yCheck = m1.position.y < m2.position.y + epsilon && m1.position.y > m2.position.y - epsilon;
	return xCheck && yCheck; 
}