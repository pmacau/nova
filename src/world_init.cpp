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
	motion.scale = vec2(sprite.dims.x, sprite.dims.y);

	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);

	renderRequest.used_texture = TEXTURE_ASSET_ID::PLAYER;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createPlayerHealthBar(entt::registry& registry) {
	auto entity = registry.create();
	registry.emplace<PlayerHealthBar>(entity);
	auto& motion = registry.emplace<Motion>(entity);
	motion.position = vec2({ WINDOW_WIDTH_PX - 170, 50});
	motion.angle = 0.f;
	motion.velocity = vec2({ 0, 0 });
	motion.scale = vec2({120, 8});
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 1, 1 };
	sprite.sheet_dims = { 0, 0 };
	auto& render_request = registry.emplace<RenderRequest>(entity);
	render_request.used_texture = TEXTURE_ASSET_ID::HEALTHBAR_GREEN;
	render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	return entity;
}

entt::entity createMob(entt::registry& registry, vec2 position) {
	auto entity = registry.create();
	auto& mob = registry.emplace<Mob>(entity);
	// dummy sprite
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 40.f, 54.f};
	sprite.sheet_dims = { 140.f, 93.f };
	sprite.coord = { 38.f / 54.f, 86.f / 40.f};
	mob.health = MOB_HEALTH;
	mob.hit_time = 1.f;
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2(sprite.dims.x, sprite.dims.y);
	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::MOB;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	createMobHealthBar(registry, entity);

	auto& drop = registry.emplace<Drop>(entity);
	drop.item_type = ITEM_TYPE::POTION;

	std::cout << "Created mob" << std::endl; 
	return entity; 
}

entt::entity createMobHealthBar(entt::registry& registry, entt::entity& mob_entity) {
	auto entity = registry.create();
	registry.emplace<MobHealthBar>(entity);
	auto& healthbar = registry.get<MobHealthBar>(entity);
	healthbar.ent = mob_entity;
	auto& motion = registry.emplace<Motion>(entity);
	auto& mob_motion = registry.get<Motion>(mob_entity);
	motion.position.x = mob_motion.position.x;
	motion.position.y = mob_motion.position.y - mob_motion.scale.y / 2 - 40;
	motion.angle = 0.f;
	motion.velocity = vec2({ 0, 0 });
	motion.scale = vec2({ std::max(30.f, mob_motion.scale.x / 3), 5}); // for boss we may want bigger health bar hence max function
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 1, 1 };
	sprite.sheet_dims = { 0, 0 };
	auto& render_request = registry.emplace<RenderRequest>(entity);
	render_request.used_texture = TEXTURE_ASSET_ID::HEALTHBAR_RED;
	render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	return entity;
}

void createInventory(entt::registry& registry) {
	auto inventory_entity = registry.create();
	auto& inventory = registry.emplace<Inventory>(inventory_entity);
	float startX = 50.0f;
	const float startY = 50.0f;
	const float SLOT_SIZE = 25.f;
	for (int i = 0; i < MAX_INVENTORY_SLOTS; i++) {
		auto entity = registry.create();
		inventory.slots.push_back(entity);
		registry.emplace<InventorySlot>(entity);
		auto& motion = registry.emplace<Motion>(entity);
		motion.angle = 0.0f;
		motion.position = {startX, startY};
		startX += 2 * SLOT_SIZE - 2;
		motion.scale = {SLOT_SIZE, SLOT_SIZE};
		motion.velocity = {0.f, 0.f};
		auto& sprite = registry.emplace<Sprite>(entity);
		sprite.coord = {0.f, 0.f};
		sprite.dims = {510.f, 510.f};
		sprite.sheet_dims = {510.f, 510.f};
		auto& render_request = registry.emplace<RenderRequest>(entity);
		render_request.used_texture = TEXTURE_ASSET_ID::INVENTORY_SLOT;
		render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
		render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	}
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