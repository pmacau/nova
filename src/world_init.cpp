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
	motion.scale = vec2(19 * 4, 31 * 4);

	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);

	renderRequest.used_texture = TEXTURE_ASSET_ID::PLAYER;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! TODO A1: implement grid lines as gridLines with renderRequests and colors
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
entt::entity createInvader(entt::registry& registry, vec2 position)
{
	// reserve an entity
	auto entity = registry.create();

	// invader
	auto& invader = registry.emplace<Invader>(entity);
	invader.health = INVADER_HEALTH;

	// TODO A1: initialize the position, scale, and physics components
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// resize, set scale to negative if you want to make it face the opposite way
	// motion.scale = vec2({ -INVADER_BB_WIDTH, INVADER_BB_WIDTH });
	motion.scale = vec2({ INVADER_BB_WIDTH, INVADER_BB_HEIGHT });

	// create an (empty) Bug component to be able to refer to all bug
	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::INVADER;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! TODO A1: create a new projectile w/ pos, size, & velocity
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity)
{
	auto entity = registry.create();

	// TODO: projectile
	// TODO: motion
	// TODO: renderRequests

	return entity;
}