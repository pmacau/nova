#include <iostream>
#include "ai_system.hpp"
#include "world_init.hpp"
#include "world_system.hpp"
#include "tinyECS/components.hpp"
	
AISystem::AISystem(entt::registry& reg) :
	registry(reg)
{
	player_entity = *reg.view<Player>().begin(); 
}


void AISystem::step(float elapsed_ms)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented

	
	/*for (const Entity& tower_entity : registry.towers.entities) {
		
	}*/

	auto mobs = registry.view<Mob>();
	Motion& player_motion = registry.get<Motion>(player_entity); // position to base pathing
	//std::cout << "player_position" << player_motion.position.x << " " << player_motion.position.y << std::endl;
	for (auto entity : mobs) {
		Motion& mob_motion = registry.get<Motion>(entity); // gets motion component of mob
		if (CollisionSystem::isContact(entity, player_entity, registry, 10)) {
			mob_motion.velocity = vec2(0, 0);
			continue; 
		}

		vec2 velo_to_player = MOB_SPEED * normalize(
			(player_motion.position + player_motion.offset_to_ground) -
			(mob_motion.position + mob_motion.offset_to_ground)
		); // direction from mob to player

		if (registry.any_of<Boss>(entity)) {
			auto& boss = registry.get<Boss>(entity);
			vec2 velo_to_home = MOB_SPEED * normalize(
				boss.spawn - (mob_motion.position + mob_motion.offset_to_ground)
			);
			float player_dist = distance(mob_motion.position, player_motion.position);
			mob_motion.velocity = (player_dist <= boss.agro_range) ? velo_to_player : velo_to_home;
		} else {
			mob_motion.velocity = velo_to_player;
		}
		// sets velocity of mob to be towards player
		//std::cout << "mob velocity" << mob_motion.velocity.x << " " << mob_motion.velocity.y << std::endl;

	}

}


float AISystem::magnitude(vec2 v) {
	float x_comp = v.x * v.x;
	float y_comp = v.y * v.y; 
	return sqrt(x_comp + y_comp);
}