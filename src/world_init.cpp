#include "world_init.hpp"
// #include "tinyECS/registry.hpp"
#include <iostream>


entt::entity createPlayer(entt::registry& registry, vec2 position)
{
	auto entity = registry.create();

	auto& animation = registry.emplace<Animation>(entity);
	animation.frameDuration = 100.0f;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = PLAYER_SPRITESHEET.dims;
	sprite.sheet_dims = PLAYER_SPRITESHEET.sheet_dims;

	auto& player = registry.emplace<Player>(entity);
	player.health = PLAYER_HEALTH;
	//player.direction = 0; // TODO: use enum
	// HITBOX
	auto& hitBox = registry.emplace<HitBox>(entity);
	hitBox.type = HitBoxType::HITBOX_CIRCLE;
	hitBox.shape.circle.radius = 25.f;
	/*hitBox.type = HitBoxType::HITBOX_RECT;
	hitBox.shape.rect.width = 43.f;
	hitBox.shape.rect.height = 55.f;*/

	 
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
	// ENTITY CREATION
	auto entity = registry.create();

	auto& mob = registry.emplace<Mob>(entity);
	// SPRITE 
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 43.f, 55.f };
	sprite.sheet_dims = {43.f, 55.f};
	// HITBOX
	auto& hitBox = registry.emplace<HitBox>(entity); 
	hitBox.type = HitBoxType::HITBOX_CIRCLE; 
	hitBox.shape.circle.radius = 40.f; 
	/*hitBox.type = HitBoxType::HITBOX_RECT;
	hitBox.shape.rect.width = 43.f;
	hitBox.shape.rect.height = 55.f;*/

	mob.health = MOB_HEALTH;
	mob.hit_time = 1.f;
	
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position.x = position.x + sprite.dims[0] / 2;
	//std::cout << sprite.dims[0] << std::endl; 
	motion.position.y = position.y + sprite.dims[1] / 2;
	motion.scale = vec2(100, 120);
	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::MOB;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	std::cout << "Created mob" << std::endl; 
	return entity; 
}

entt::entity createRockType1(entt::registry& registry, vec2 position) {
	auto entity = registry.create();
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 5.f, 5.f };
	sprite.sheet_dims = { 144.f, 135.f };
	auto& hitBox = registry.emplace<HitBox>(entity);
	hitBox.type = HitBoxType::HITBOX_RECT;
	hitBox.shape.rect.width = 54.f;
	hitBox.shape.rect.height = 54.f;
	/*hitBox.type = HitBoxType::HITBOX_CIRCLE;
	hitBox.shape.circle.radius = 12.f;*/
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2(54, 54);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	auto& obstacle = registry.emplace<Obstacle>(entity);
	obstacle.isPassable = false;

	renderRequest.used_texture = TEXTURE_ASSET_ID::STONE_BLOCK_1;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	return entity;

}


entt::entity createTreeType1(entt::registry& registry, vec2 position) {
	auto entity = registry.create();
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 80.f, 80.f };
	sprite.sheet_dims = { 800.f, 944.f };
	auto& hitBox = registry.emplace<HitBox>(entity);
	/*hitBox.type = HitBoxType::HITBOX_RECT;
	hitBox.shape.rect.width = 54.f;
	hitBox.shape.rect.height = 54.f;*/
	hitBox.type = HitBoxType::HITBOX_CIRCLE;
	hitBox.shape.circle.radius = 115.f;
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2(250, 250);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	auto& obstacle = registry.emplace<Obstacle>(entity);
	obstacle.isPassable = false;

	renderRequest.used_texture = TEXTURE_ASSET_ID::TREE;
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
	motion.scale = vec2(19 * 14, 35 * 4.5);
	auto& hitBox = registry.emplace<HitBox>(entity);
	hitBox.type = HitBoxType::HITBOX_RECT;
	hitBox.shape.rect.width = 19.f * 11.f;
	hitBox.shape.rect.height = 35.f * 3.2;

	auto& obstacle = registry.emplace<Obstacle>(entity);
	obstacle.isPassable = false;

	std::cout << "Ship position: " << position.x << ", " << position.y << std::endl;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0.0f, 0.0f};
    // sprite.dims = {19 * 15, 35 * 7};
	sprite.dims = {128, 75};
    sprite.sheet_dims = { 128, 75 };

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::SHIP;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity)
{
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