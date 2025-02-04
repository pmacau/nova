#pragma once

#include "common.hpp"
#include <entt.hpp>
// #include "tinyECS/tiny_ecs.hpp"
#include "render_system.hpp"

// invaders
entt::entity createInvader(entt::registry& registry, vec2 position);

// towers
entt::entity createTower(entt::registry& registry, vec2 position);
void removeTower(entt::registry& registry, vec2 position);

// projectile
entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity);

// grid lines to show tile positions
entt::entity createGridLine(entt::registry& registry, vec2 start_pos, vec2 end_pos);

// debugging red lines
entt::entity createLine(entt::registry& registry, vec2 position, vec2 size);