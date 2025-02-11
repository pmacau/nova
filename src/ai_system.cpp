#include <iostream>
#include "ai_system.hpp"
#include "world_init.hpp"
#include "world_system.hpp"
#include "tinyECS/components.hpp"

AISystem::AISystem(entt::registry& reg) :
	registry(reg)
{
}


void AISystem::step(float elapsed_ms)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented

	
	/*for (const Entity& tower_entity : registry.towers.entities) {
		
	}*/

	auto mobs = registry.view<Mob>();
	auto player = registry.view<Player>(); 

	Motion& player_motion = registry.get<Motion>(player.front()); // position to base pathing
	//std::cout << "player_position" << player_motion.position.x << " " << player_motion.position.y << std::endl;
	for (auto entity : mobs) {
		Motion& mob_motion = registry.get<Motion>(entity); // gets motion component of mob
		if (CollisionSystem::isContact(entity, player.front(), registry, 10)) {
			mob_motion.velocity = vec2(0, 0);
			continue; 
		}
		vec2 direction = player_motion.position - mob_motion.position; 
		if (player_motion.position - mob_motion.position == vec2(0, 0)) { //prevents undefined when dividing by 0
			direction = vec2(0, 0);
		}
		else {
			direction = normalize(direction) * MOB_SPEED; 
		}
		mob_motion.velocity = direction; // sets velocity of mob to be towards player
		std::cout << "mob velocity" << mob_motion.velocity.x << " " << mob_motion.velocity.y << std::endl;

	}

}


float AISystem::magnitude(vec2 v) {
	float x_comp = v.x * v.x;
	float y_comp = v.y * v.y; 
	return sqrt(x_comp + y_comp);
}