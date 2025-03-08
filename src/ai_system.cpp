#include <iostream>
#include "ai_system.hpp"
#include "world_init.hpp"
#include "world_system.hpp"
#include "tinyECS/components.hpp"
#include "music_system.hpp"
#include "ai/ai_component.hpp"
	
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

	// auto mobs = registry.view<Mob>();
	// Motion& player_motion = registry.get<Motion>(player_entity); // position to base pathing
	// for (auto entity : mobs) {
	// 	Motion& mob_motion = registry.get<Motion>(entity); // gets motion component of mob
	// 	if (CollisionSystem::isContact(entity, player_entity, registry, 10)) {
	// 		mob_motion.velocity = vec2(0, 0);
	// 		continue; 
	// 	}

	// 	vec2 velo_to_player = MOB_SPEED * normalize(
	// 		(player_motion.position + player_motion.offset_to_ground) -
	// 		(mob_motion.position + mob_motion.offset_to_ground)
	// 	); // direction from mob to player

	// 	if (registry.any_of<Boss>(entity)) {
	// 		auto& boss = registry.get<Boss>(entity);
	// 		vec2 velo_to_home = MOB_SPEED * normalize(
	// 			boss.spawn - (mob_motion.position + mob_motion.offset_to_ground)
	// 		);
	// 		float player_dist = distance(mob_motion.position, player_motion.position);
	// 		float home_dist = distance(mob_motion.position, boss.spawn);

	// 		if (player_dist <= boss.agro_range) {
	// 			mob_motion.velocity = velo_to_player;
	// 		} else if (home_dist <= 32) {
	// 			mob_motion.velocity = {0.f, 0.f};
	// 		} else {
	// 			mob_motion.velocity = velo_to_home;
	// 		}
	// 	} else {
	// 		mob_motion.velocity = velo_to_player;
	// 	}
	// }


	auto view = registry.view<AIComponent>();
    for (auto entity : view) {
        auto& aiComp = view.get<AIComponent>(entity);
        if (aiComp.stateMachine) {
            aiComp.stateMachine->update(elapsed_ms);
        }
    }

}


float AISystem::magnitude(vec2 v) {
	float x_comp = v.x * v.x;
	float y_comp = v.y * v.y; 
	return sqrt(x_comp + y_comp);
}