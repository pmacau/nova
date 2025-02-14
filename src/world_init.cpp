#include "world_init.hpp"
// #include "tinyECS/registry.hpp"
#include <iostream>

entt::entity createPlayer(entt::registry& registry, vec2 position)
{
	auto entity = registry.create();

	auto& animation = registry.emplace<Animation>(entity);
	animation.frameDuration = 100.0f;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = {19.f, 32.f};
	sprite.sheet_dims = {152.f, 96.f};

	auto& player = registry.emplace<Player>(entity);
	player.health = PLAYER_HEALTH;
	player.direction = 0; // TODO: use enum

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = vec2(19 * 2, 32 * 2);

	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);

	renderRequest.used_texture = TEXTURE_ASSET_ID::PLAYER;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createMob(entt::registry& registry, vec2 position) {
	auto entity = registry.create();
	auto& mob = registry.emplace<Mob>(entity);
	// dummy sprite
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 140.f, 93.f };
	sprite.sheet_dims = { 140.f, 93.f };
	mob.health = MOB_HEALTH;
	mob.hit_time = 1.f;
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2(19 * 25, 31 * 12);
	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::MOB;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	std::cout << "Created mob" << std::endl; 
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

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0.0f, 0.0f};
    // sprite.dims = {19 * 15, 35 * 7};
	sprite.dims = {19 * 22, 35 * 7};
    sprite.sheet_dims = position;

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
	std::cout << "Created Projectile" << std::endl; 
	auto entity = registry.create();

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = {18.f, 18.f};
	sprite.sheet_dims = {18.f, 18.f};

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