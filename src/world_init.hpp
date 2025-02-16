#pragma once

#include "common.hpp"
#include <entt.hpp>
//#include "tinyECS/tiny_ecs.hpp"
#include "render_system.hpp"
//#include "tinyECS/components.hpp"

// must be updated and removed accordingly. 
// TODO: must implement removal. 
inline std::unordered_set<entt::entity> mobMap;
inline std::unordered_set<entt::entity> playerMap; 
inline std::unordered_set<entt::entity> projectileMap; 


entt::entity createPlayer(entt::registry& registry, vec2 position);

// invaders
entt::entity createMob(entt::registry& registry, vec2 position);

// projectile
entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity);