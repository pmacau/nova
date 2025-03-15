#include <unordered_set>

#include "collision_system.hpp"
#include "ui_system.hpp"
#include "music_system.hpp"
#include "util/debug.hpp"

CollisionSystem::CollisionSystem(entt::registry& reg, WorldSystem& world, PhysicsSystem& physics) :
	registry(reg),
	physics(physics),
	world(world)
{
	
	
}

void CollisionSystem::initTree(int mapWidth, int mapHeight) {
	quadTree = new QuadTree(100.0f * 16.f, 100.0f * 16.f, 200 * 16.f, 200 * 16.f); // NEED TO CHANGE
	auto view = registry.view<Motion, Hitbox>(); //currently reloads entire tree per frame
	for (auto entity : view) {
		//std::cout << "woo" << std::endl;
		quadTree->insert(entity, registry);
	}
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
	auto view = registry.view<Motion, Hitbox>(); //currently reloads entire tree per frame
	/*for (auto entity : view) {
		auto motion = registry.get<Motion>(entity); 
		if (glm::length(motion.position - registry.get<Motion>(playerView.front()).position) < 40) {
			if (motion.formerPosition != motion.position) {
				quadTree->remove(entity, registry);
				quadTree->insert(entity, registry);
			}
		}	
	}*/

	//Get the player entity and its position
	auto playerEntity = playerView.front();
	const auto& playerMotion = registry.get<Motion>(playerEntity);

	// Create a query range around the player
	// The range is a square centered on the player - adjust the size as needed
	const float queryRange = 1000.f; // Adjust based on your game's scale
	Quad rangeQuad(
		playerMotion.position.x,
		playerMotion.position.y,
		queryRange,
		queryRange
	);

	//creating a query for all entities in range of player screen
	std::vector<entt::entity> nearbyEntities = quadTree->queryRange(rangeQuad, registry);
	
	
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
				resolve(e1, e2, elapsed_ms);
				processHandler(e1, e2);
			}
		}
	}

	
	for (auto entity : destroy_entities) registry.destroy(entity);


	/*for (auto&& [e1, m1, h1] : registry.view<Motion, Hitbox>().each()) {
		for (auto &&[e2, m2, h2]: registry.view<Motion, Hitbox>().each()) {
			if (
				e1 == e2 ||
				processed.find(e1) != processed.end() ||
				processed.find(e2) != processed.end() ||
				!collides(h1, m1, h2, m2)
			) continue;

			resolve(e1, e2, elapsed_ms);

			processHandler(e1, e2); 
		}
	}

	 for (auto entity: destroy_entities) registry.destroy(entity);*/
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
				destroy_entities.push_back(hb_ent);
				break;
			}
		}
		UISystem::renderItem(registry, mob_ent);
		destroy_entities.push_back(mob_ent);
	}
	destroy_entities.push_back(proj_ent);
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

template<>
void CollisionSystem::handle<Projectile, Obstacle>(
	entt::entity proj_ent, entt::entity obs_ent, float elapsed_ms
) {
	destroy_entities.push_back(proj_ent);
}

void CollisionSystem::resolve(entt::entity e1, entt::entity e2, float elapsed_ms) {
	if      (collision_type<Player, Mob>(e1, e2))      handle<Player, Mob>(e1, e2, elapsed_ms);
	else if (collision_type<Projectile, Mob>(e1, e2))  handle<Projectile, Mob>(e1, e2, elapsed_ms);
	else if (collision_type<Projectile, Obstacle>(e1, e2)) handle<Projectile, Obstacle>(e1, e2, elapsed_ms); 
	// TODO: when AI gets improved, make all mobs unable to walk into obstacles
	else if (collision_type<Obstacle, Player>(e1, e2)) handle<Obstacle, Player>(e1, e2, elapsed_ms);
}