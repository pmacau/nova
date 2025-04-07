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

	auto view = registry.view<AIComponent>();
    for (auto entity : view) {
        auto& aiComp = view.get<AIComponent>(entity);
		aiComp.attackCooldownTimer += elapsed_ms;

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