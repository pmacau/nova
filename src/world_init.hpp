#pragma once

#include "common.hpp"
#include <entt.hpp>
#include "render_system.hpp"


entt::entity createPlayer(entt::registry& registry, vec2 position);

entt::entity createShip(entt::registry& registry, vec2 position);

// invaders
entt::entity createMob(entt::registry& registry, vec2 position);

// projectile
entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity);

// terrain
entt::entity createRockType1(entt::registry& registry, vec2 position);