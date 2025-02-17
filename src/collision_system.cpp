#include "collision_system.hpp"


CollisionSystem::CollisionSystem(entt::registry& reg, WorldSystem& world, PhysicsSystem& physics) :
	registry(reg),
	world(world), 
	physics(physics)
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
	auto entities = registry.view<Motion, HitBox>(); // only goes through motion objects with HitBox (should essentially be all of them)
	auto playerCheck = registry.view<Player>();
	for (auto entity : entities) { // does a for loop for projectiles and what not right, now this isn't necessary. 
		// only check once so decide if checks on player or invader (player chosen)
		// checks if ID is player
		if ((uint32_t)entt::entt_traits<entt::entity>::to_entity(playerCheck.front()) == (uint32_t)entt::entt_traits<entt::entity>::to_entity(entity)) {
			for (auto mob : mobs) {
				//std::cout << "ENTERED" << std::endl;
				if (isContact(mob, entity, registry, 40)) {
					auto& player_ref = registry.get<Player>(entity);
					auto& mob_ref = registry.get<Mob>(mob);
					if (mob_ref.hit_time <= 0) {
						std::cout << "COLLISION" << std::endl;
						player_ref.health -= MOB_DAMAGE;
						if (player_ref.health <= 0) {
							world.player_respawn();
						}
						mob_ref.hit_time = 1.f;
					}
					// repelling force 
					physics.suppress(entity, mob);
				}
				
				
				


			}
		}

	}
}


	
	// refactor this so it uses the hitboxes.
	// ASSUMES ENTIT
	// determines if should get hit or not maybe just refactor into hit box system after.
	bool CollisionSystem::isContact(entt::entity e1, entt::entity e2, entt::registry & registry, float epsilon) {
		Motion m1 = registry.get<Motion>(e1);
		Motion m2 = registry.get<Motion>(e2);
		HitBox h1 = registry.get<HitBox>(e1); // can be a circle or a rectangle
		HitBox h2 = registry.get<HitBox>(e2); 
	
		// cases two circles, 
		bool xCheck = m1.position.x < m2.position.x + epsilon && m1.position.x > m2.position.x - epsilon;
		bool yCheck = m1.position.y < m2.position.y + epsilon && m1.position.y > m2.position.y - epsilon;
		return xCheck && yCheck;
	}
