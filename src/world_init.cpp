#include "world_init.hpp"
// #include "tinyECS/registry.hpp"
#include <iostream>

entt::entity createPlayer(entt::registry& registry, vec2 position)
{
	auto entity = registry.create();
	auto& player = registry.emplace<Player>(entity);
	player.health = PLAYER_HEALTH;

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = vec2(19 * 2, 31 * 2);

	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);

	renderRequest.used_texture = TEXTURE_ASSET_ID::PLAYER;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createShip(entt::registry& registry, vec2 position)
{
	auto entity = registry.create();
	auto& ship = registry.emplace<Ship>(entity);
	ship.health = SHIP_HEALTH;
	ship.range = SHIP_RANGE;
	ship.timer = SHIP_TIMER_MS;

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = vec2(19 * 13, 35 * 7);

	std::cout << "Ship position: " << position.x << ", " << position.y << std::endl;


	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);

	renderRequest.used_texture = TEXTURE_ASSET_ID::SHIP;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

// // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// // !!! TODO A1: implement grid lines as gridLines with renderRequests and colors
// // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// entt::entity createInvader(entt::registry& registry, vec2 position)
// {
// 	// reserve an entity
// 	auto entity = registry.create();

// 	// invader
// 	auto& invader = registry.emplace<Invader>(entity);
// 	invader.health = INVADER_HEALTH;

// 	// TODO A1: initialize the position, scale, and physics components
// 	auto& motion = registry.emplace<Motion>(entity);
// 	motion.angle = 0.f;
// 	motion.velocity = { 0, 0 };
// 	motion.position = position;

// 	// resize, set scale to negative if you want to make it face the opposite way
// 	// motion.scale = vec2({ -INVADER_BB_WIDTH, INVADER_BB_WIDTH });
// 	motion.scale = vec2({ INVADER_BB_WIDTH, INVADER_BB_HEIGHT });

// 	// create an (empty) Bug component to be able to refer to all bug
// 	registry.emplace<Eatable>(entity);
// 	auto& renderRequest = registry.emplace<RenderRequest>(entity);
// 	renderRequest.used_texture = TEXTURE_ASSET_ID::INVADER;
// 	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
// 	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

// 	return entity;
// }

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! TODO A1: create a new projectile w/ pos, size, & velocity
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity)
{
	auto entity = registry.create();

	auto& projectile = registry.emplace<Projectile>(entity);
	projectile.damage = PROJECTILE_DAMAGE;

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = velocity;
	motion.position = pos;
	motion.scale = size;

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::GOLD_PROJECTILE;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}