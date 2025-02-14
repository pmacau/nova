#include "collision_system.hpp"
#include "world_init.hpp"

CollisionSystem::CollisionSystem(entt::registry& reg, WorldSystem& world) :
	registry(reg),
	world(world)
{
}

void CollisionSystem::impossibleMovements() {

}

void CollisionSystem::updatePlayerHealthBar(int health) {
	for (auto entity : registry.view<PlayerHealthBar>()) {
		auto& playerhealth_motion = registry.get<Motion>(entity);
		float left = playerhealth_motion.position.x - playerhealth_motion.scale.x;
		playerhealth_motion.scale = vec2({ health * (120.f / PLAYER_HEALTH), 8});
		playerhealth_motion.position.x = left + playerhealth_motion.scale.x; // to keep the healthbar fixed to left because scaling happens relative to center
		break; // since there is only one healthbar, is there a better way instead of doing the loop?
	}
}

void CollisionSystem::updateMobHealthBar(entt::entity& mob_entity) {
	auto& mob = registry.get<Mob>(mob_entity);
	auto& mob_motion = registry.get<Motion>(mob_entity);
	for (auto entity : registry.view<MobHealthBar>()) {
		auto& healthbar = registry.get<MobHealthBar>(entity);
		if (healthbar.ent == mob_entity) {
			auto& mobhealth_motion = registry.get<Motion>(entity);
			float left = mobhealth_motion.position.x - mobhealth_motion.scale.x;
			mobhealth_motion.scale = vec2({ mob.health * std::max(30.f, mob_motion.scale.x / 3) / MOB_HEALTH, 5 }); 
			mobhealth_motion.position.x = left + mobhealth_motion.scale.x;
			break;
		}
	}
}


void CollisionSystem::step(float elapsed_ms)
{
	std::vector<entt::entity> destroy_entities;
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
				if (isContact(entity, mob, registry, 30.f)){
					
					auto& player_ref = registry.get<Player>(entity);
					auto& mob_ref = registry.get<Mob>(mob);
					if (mob_ref.hit_time <= 0) {
						std::cout << "COLLISION" << std::endl;
						player_ref.health -= MOB_DAMAGE;
						updatePlayerHealthBar(player_ref.health);
						if (player_ref.health <= 0) {
							updatePlayerHealthBar(PLAYER_HEALTH);
							world.player_respawn();
						}
						mob_ref.hit_time = 1.f;
					}

				}
			}
		}
		if (registry.all_of<Projectile>(entity)) {
			auto& projectile = registry.get<Projectile>(entity);
			for (auto mob_entity : mobs) {
				auto& mob = registry.get<Mob>(mob_entity);
				if (isContact(entity, mob_entity, registry, 10.f)) {
					registry.destroy(entity);
					mob.health -= projectile.damage;
					updateMobHealthBar(mob_entity);
					if (mob.health <= 0) {
						for (auto healthbar_entity : registry.view<MobHealthBar>()) {
							auto& healthbar = registry.get<MobHealthBar>(healthbar_entity);
							std::cout << (healthbar.ent == mob_entity) << "\n";
							if (healthbar.ent == mob_entity) {
								destroy_entities.push_back(healthbar_entity); 
								break;
							}
						}
						destroy_entities.push_back(mob_entity);
					}
				}
			}
		}
	}
	for (auto entity : destroy_entities) {
		registry.destroy(entity);
	}
}


// determines if tw
bool CollisionSystem::isContact(entt::entity e1 , entt::entity e2, entt::registry& registry, float adjust) {
	Motion m1 = registry.get<Motion>(e1); 
	Motion m2 = registry.get<Motion>(e2);
	bool xCheck = m1.position.x > m2.position.x - m2.scale.x / 2.f - m1.scale.x / 2.f - adjust && m1.position.x < m2.position.x + m2.scale.x /  2.f + m1.scale.x / 2.f + adjust;
	bool yCheck = m1.position.y > m2.position.y - m2.scale.y / 2.f - m1.scale.y / 2.f - adjust && m1.position.y < m2.position.y + m2.scale.y / 2.f + m1.scale.y / 2.f + adjust;
	return xCheck && yCheck; 
}