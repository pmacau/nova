#include "collision_system.hpp"
// stlib
#include <cassert>
#include <sstream>
#include <iostream>

CollisionSystem::CollisionSystem(entt::registry& reg, WorldSystem& world) :
	registry(reg),
	world(world)
{
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
				std::cout << "ENTERED" << std::endl;
				Motion mob_position = registry.get<Motion>(mob);
				Motion player_position = registry.get<Motion>(entity);
				bool xCheck = mob_position.position.x < player_position.position.x + 10 && mob_position.position.x > player_position.position.x - 10;
				bool yCheck = mob_position.position.y < player_position.position.y + 10 && mob_position.position.y > player_position.position.y - 10;
				if (xCheck && yCheck) {
					std::cout << "COLLISION" << std::endl;
					auto& player_ref = registry.get<Player>(entity);
					auto& mob_ref = registry.get<Mob>(mob);
					if (mob_ref.hit_time <= 0) {
						player_ref.health -= MOB_DAMAGE;
						if (player_ref.health <= 0) {
							world.player_respawn();
						}
						mob_ref.hit_time = 1.f;
					}

				}


			}
		}

	}
}