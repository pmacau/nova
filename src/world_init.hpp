#pragma once

#include "common.hpp"
#include <entt.hpp>
// #include "tinyECS/tiny_ecs.hpp"
#include "render_system.hpp"


entt::entity createPlayer(entt::registry& registry, vec2 position);

// invaders
entt::entity createMob(entt::registry& registry, vec2 position);

// projectile
entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity);

entt::entity createPlayerHealthBar(entt::registry& registry);

entt::entity createMobHealthBar(entt::registry& registry, entt::entity& mob_entity);