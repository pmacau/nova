#pragma once

#include "common.hpp"
#include <entt.hpp>
#include "render_system.hpp"
#include "ai/ai_common.hpp"


entt::entity createPlayer(entt::registry& registry, vec2 position);

entt::entity createPlayerHealthBar(entt::registry& registry, vec2 position);

entt::entity createShip(entt::registry& registry, vec2 position);

entt::entity createUIShip(entt::registry& registry, vec2 position, vec2 scale, int shipNum);

// invaders
entt::entity createMob(entt::registry& registry, vec2 position, int health = MOB_HEALTH);
entt::entity createMob2(entt::registry& registry, vec2 position, int health = MOB_HEALTH);

entt::entity createMobHealthBar(entt::registry& registry, entt::entity& mob_entity);

// projectile
entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity);


entt::entity createBoss(entt::registry& registry, vec2 pos);
entt::entity createTree(entt::registry& registry, vec2 pos, FrameIndex spriteCoord);

entt::entity createTextBox(entt::registry& registry, vec2 position, vec2 size, std::string text, float scale, vec3 textColor);

// terrain
// entt::entity createRockType1(entt::registry& registry, vec2 position);

// entt::entity createTreeType1(entt::registry& registry, vec2 position);

// camera
entt::entity createCamera(entt::registry& registry, entt::entity target);

void createInventory(entt::registry& registry);

void destroy_creature(entt::registry& registry, entt::entity creature);

entt::entity createCreature(entt::registry& registry, vec2 position, CreatureType creatureType, int health);