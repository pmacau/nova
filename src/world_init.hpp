#pragma once

#include "common.hpp"
#include <entt.hpp>
#include "render_system.hpp"


entt::entity createPlayer(entt::registry& registry, vec2 position);

entt::entity createPlayerHealthBar(entt::registry& registry, vec2 position);

entt::entity createShip(entt::registry& registry, vec2 position);

// invaders
entt::entity createMob(entt::registry& registry, vec2 position);

// projectile
entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity);

// camera
entt::entity createCamera(entt::registry& registry, entt::entity target);