#pragma once

#include "common.hpp"
#include "render_system.hpp"
#include <entt.hpp>
#include "collision_system.hpp"

class AISystem {
public:
    AISystem(entt::registry& reg);
    void step(float elapsed_ms);

private:
    entt::registry& registry;
	float magnitude(vec2 v);
	float movementEpsilon = 0.2f; // Epsilon for movement
    entt::entity player_entity; 
};