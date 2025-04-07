#include <unordered_set>

#include "collision_system.hpp"
#include "ui_system.hpp"
#include "music_system.hpp"
#include "util/debug.hpp"


CollisionSystem::CollisionSystem(entt::registry& reg, WorldSystem& world, PhysicsSystem& physics, QuadTree& quadTree, SpawnSystem& spawnSystem, FlagSystem& flagSystem) :
	registry(reg),
	physics(physics),
	world(world),
	quadTree(quadTree),
	spawnSystem(spawnSystem), 
	flagSystem(flagSystem)
{
	
	
}



void CollisionSystem::step(float elapsed_ms) {
	processed.clear();
	destroy_entities.clear();
	//quadTree->clear(); 

	
	
	// Find the player entity
	auto playerView = registry.view<Player, Motion>();
	if (playerView.begin() == playerView.end()) {
		return;
	}
		
	

	//Get the player entity and its position
	auto playerEntity = playerView.front();
	const auto& playerMotion = registry.get<Motion>(playerEntity);

	// Create a query range around the player
	// The range is a square centered on the player - adjust the size as needed
	const float queryRange = WINDOW_WIDTH_PX * 1.25f; // Adjust based on your game's scale
	Quad rangeQuad(
		playerMotion.position.x,
		playerMotion.position.y,
		queryRange,
		queryRange
	);

	//creating a query for all entities in range of player screen
	std::vector<entt::entity> nearbyEntities = quadTree.quadTree->queryRange(rangeQuad, registry);
	auto mobs = registry.view<Mob>(); 
	for (auto mob : mobs) {
		nearbyEntities.push_back(mob); 
	}
	auto projectiles = registry.view<Projectile>(); 
	for (auto projectile : projectiles) {
		nearbyEntities.push_back(projectile); 
	}
	auto slashes = registry.view<Slash>(); 
	for (auto slash : slashes) {
		nearbyEntities.push_back(slash); 
	}
	nearbyEntities.push_back(playerEntity); 
	
	//std::cout << nearbyEntities.size() << std::endl;

	for (size_t i = 0; i < nearbyEntities.size(); ++i) {
		auto e1 = nearbyEntities[i];
		// Skip if this entity has already been processed
		if (processed.find(e1) != processed.end()) continue;

		const auto& m1 = registry.get<Motion>(e1);
		const auto& h1 = registry.get<Hitbox>(e1);

		for (size_t j = i + 1; j < nearbyEntities.size(); ++j) {
			auto e2 = nearbyEntities[j];

			if (processed.find(e2) != processed.end()) continue;

			const auto& m2 = registry.get<Motion>(e2);
			const auto& h2 = registry.get<Hitbox>(e2);

			if (collides(h1, m1, h2, m2)) {
				//std::cout << "coo" << std::endl; 
				resolve(e1, e2, elapsed_ms);
				processHandler(e1, e2);
			}
		}
	}

	
	for (auto entity : destroy_entities) {
		if (registry.valid(entity)) {
			if (registry.any_of<Mob>(entity)) {
				for (BossSpawn& boss : spawnSystem.bossSpawnData) {
					flagSystem.bossDefeatedHelper(boss.creatureID);
					if (boss.entity == entity) {
						boss.defeated = true;
						boss.entity = entt::null;
						break;
					}
					
				}
				registry.destroy(entity);
			}
		}
	}
	for (auto slash : slashes) {
		if (!registry.valid(slash)) {
			continue; // Skip if the entity is not valid
		}
		Slash& s = registry.get<Slash>(slash); 
		s.hit = true;
	}
}

void CollisionSystem::processHandler(entt::entity& e1, entt::entity& e2) {
	if (registry.all_of<Projectile>(e1)) processed.insert(e1);
	if (registry.all_of<Projectile>(e2)) processed.insert(e2);
}

template<typename C1, typename C2>
bool CollisionSystem::collision_type(entt::entity& e1, entt::entity& e2) {
	if (registry.all_of<C1>(e1) && registry.all_of<C2>(e2)) {
		return true;
	}
	else if (registry.all_of<C1>(e2) && registry.all_of<C2>(e1)) {
		std::swap(e1, e2);
		return true;
	}
	return false;
}


template<>
void CollisionSystem::handle<Player, Mob>(
	entt::entity play_ent, entt::entity mob_ent, float elapsed_ms
) {
	auto& player = registry.get<Player>(play_ent);
	auto& mob = registry.get<Mob>(mob_ent);
	auto& screen = registry.get<ScreenState>(registry.view<ScreenState>().front());

	if (mob.hit_time > 0) return;

	debug_printf(DebugType::COLLISION, "Player-mob collision!\n");
	mob.hit_time = 1.f; // TODO: make this a constant
	player.health -= MOB_DAMAGE;
	MusicSystem::playSoundEffect(SFX::HIT);

	UISystem::updatePlayerHealthBar(registry, player.health);
	physics.knockback(play_ent, mob_ent, 300);
	physics.suppress(play_ent, mob_ent);
	
	screen.darken_screen_factor = std::min(screen.darken_screen_factor + 0.33f, 1.0f);
}

template<>
void CollisionSystem::handle<Projectile, Mob>(
	entt::entity proj_ent, entt::entity mob_ent, float elapsed_ms
) {
	auto& projectile = registry.get<Projectile>(proj_ent);
	auto& mob = registry.get<Mob>(mob_ent);

	debug_printf(DebugType::COLLISION, "Bullet-mob collision!\n");
	mob.health -= projectile.damage;
	MusicSystem::playSoundEffect(SFX::HIT);

	UISystem::updateMobHealthBar(registry, mob_ent, true);
	if (mob.health <= 0) {
		for (auto &&[hb_ent, healthbar] : registry.view<MobHealthBar>().each()) {
			if (healthbar.entity == mob_ent) {
				destroy_entities.insert(hb_ent);
				break;
			}
		}
		if (registry.any_of<Drop>(mob_ent)) {
			UISystem::mobDrop(registry, mob_ent);
		}
		destroy_entities.insert(mob_ent);
	}
	destroy_entities.insert(proj_ent);
}

// could've probably written the logic much clearer. 
template<>
void CollisionSystem::handle<Obstacle, Player>(
	entt::entity obs_ent, entt::entity e2, float elapsed_ms
) {
	auto& obstacle = registry.get<Obstacle>(obs_ent);
	if (!obstacle.isPassable) {
		auto& motion = registry.get<Motion>(e2);
		glm::vec2 invalidPosition = motion.position; 
		motion.position = motion.formerPosition;
		motion.position.x = invalidPosition.x;
		if (!collides(registry.get<Hitbox>(obs_ent), registry.get<Motion>(obs_ent), registry.get<Hitbox>(e2), registry.get<Motion>(e2))) {
			return; // Collision resolved by correcting x-axis
		}
		motion.position.x = motion.formerPosition.x;
		motion.position.y = invalidPosition.y;
		if (!collides(registry.get<Hitbox>(obs_ent), registry.get<Motion>(obs_ent), registry.get<Hitbox>(e2), registry.get<Motion>(e2))) {
			return; // Collision resolved by correcting y-axis
		}
		motion.position = motion.formerPosition;
	}
}

// template<>
// void CollisionSystem::handle<Obstacle, Motion>(
// 	entt::entity obs_ent, entt::entity e2, float elapsed_ms
// ) {
// 	auto& obstacle = registry.get<Obstacle>(obs_ent);
// 	if (!obstacle.isPassable) {
// 		MusicSystem::playSoundEffect(SFX::WOOD);
// 	}
// }

template<>
void CollisionSystem::handle<Projectile, Obstacle>(
	entt::entity proj_ent, entt::entity obs_ent, float elapsed_ms
) {
	if (!registry.any_of<Ship>(obs_ent)) {
		destroy_entities.insert(proj_ent);
	}
}

template<>
void CollisionSystem::handle<Slash, Mob>(
	entt::entity slash_ent, entt::entity mob_ent, float elapsed_ms
) {
	auto& slash = registry.get<Slash>(slash_ent); 
	auto& mob = registry.get<Mob>(mob_ent);

	if (slash.hit) {
		return; 
	}

	debug_printf(DebugType::COLLISION, "Slash-mob collision!\n");
	mob.health -= slash.damage;
	MusicSystem::playSoundEffect(SFX::HIT);

	UISystem::updateMobHealthBar(registry, mob_ent, true);
	if (mob.health <= 0) {
		for (auto&& [hb_ent, healthbar] : registry.view<MobHealthBar>().each()) {
			if (healthbar.entity == mob_ent) {
				destroy_entities.insert(hb_ent);
				break;
			}
		}
		if (registry.any_of<Drop>(mob_ent)) {
			UISystem::mobDrop(registry, mob_ent);
		}
		destroy_entities.insert(mob_ent);
	}
	else {
		auto player_ent = registry.view<Player>().front();
		physics.knockback(mob_ent, player_ent, slash.force); 
	}
}

void CollisionSystem::resolve(entt::entity e1, entt::entity e2, float elapsed_ms) {
	if (collision_type<Player, Mob>(e1, e2))      handle<Player, Mob>(e1, e2, elapsed_ms);
	else if (collision_type<Projectile, Mob>(e1, e2))  handle<Projectile, Mob>(e1, e2, elapsed_ms);
	else if (collision_type<Projectile, Obstacle>(e1, e2)) handle<Projectile, Obstacle>(e1, e2, elapsed_ms);
	else if (collision_type<Slash, Mob>(e1, e2)) handle<Slash, Mob>(e1, e2, elapsed_ms); 
	// TODO: when AI gets improved, make all mobs unable to walk into obstacles
	else if (collision_type<Obstacle, Player>(e1, e2)) handle<Obstacle, Player>(e1, e2, elapsed_ms);
	// else if (collision_type<Obstacle, Motion>(e1, e2)) handle<Obstacle, Motion>(e1, e2, elapsed_ms);
}