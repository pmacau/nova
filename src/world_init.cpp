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
	// 
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.formerPosition = position;
	motion.scale = GAME_SCALE * PLAYER_SPRITESHEET.dims;
	// motion.scale = vec2(19 * 2, 32 * 2);
	motion.offset_to_ground = {0, motion.scale.y / 2.f};

	registry.emplace<Eatable>(entity);
	auto& renderRequest = registry.emplace<RenderRequest>(entity);

	renderRequest.used_texture = TEXTURE_ASSET_ID::PLAYER;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createPlayerHealthBar(entt::registry& registry, vec2 position) {
	auto entity = registry.create();
	registry.emplace<FixedUI>(entity);
	registry.emplace<UI>(entity);
	registry.emplace<PlayerHealthBar>(entity);
	auto& motion = registry.emplace<Motion>(entity);
	/*motion.position = { position.x + WINDOW_WIDTH_PX / 2 -  175.f,
						position.y - WINDOW_HEIGHT_PX / 2 + 50.f};*/
	motion.position = { WINDOW_WIDTH_PX - 175.F, 50.F };
	motion.angle = 0.f;
	motion.velocity = vec2({ 0, 0 });
	motion.scale = vec2({ 250.f, 15.f });
	motion.offset_to_ground = vec2(0, motion.scale.y / 2.f);
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 1152.f, 648.f };
	sprite.sheet_dims = { 1152.f, 648.f };
	auto& render_request = registry.emplace<RenderRequest>(entity);
	render_request.used_texture = TEXTURE_ASSET_ID::HEALTHBAR_GREEN;
	render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	return entity;
}

entt::entity createCamera(entt::registry& registry, entt::entity target)
{
	auto entity = registry.create();

	auto& camera = registry.emplace<Camera>(entity);
	camera.target = target;

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

	mob.health = MOB_HEALTH;
	mob.hit_time = 1.f;
	
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	// motion.position = position;

	motion.position.x = position.x + sprite.dims[0] / 2;
	//std::cout << sprite.dims[0] << std::endl; 
	motion.position.y = position.y + sprite.dims[1] / 2;
	motion.scale = vec2(100, 120);

	auto& hitBox = registry.emplace<HitBox>(entity);
	hitBox.type = HitBoxType::HITBOX_CIRCLE;
	hitBox.shape.circle.radius = 40.f; 
	//hitBox.shape.circle.radius = motion.scale.x / 2;

	// motion.scale = vec2(GAME_SCALE * 40.f, GAME_SCALE * 54.f);
	//motion.scale = vec2(38*3, 54*3);
	motion.offset_to_ground = {0, motion.scale.y / 2.f};

	registry.emplace<Eatable>(entity);
	
	auto& drop = registry.emplace<Drop>(entity);
	drop.item_type = ITEM_TYPE::POTION;

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::MOB;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	std::cout << "Created mob" << std::endl; 
	createMobHealthBar(registry, entity);
	return entity; 
}

entt::entity createMobHealthBar(entt::registry& registry, entt::entity& mob_entity) {
	auto entity = registry.create();
	registry.emplace<UI>(entity);
	registry.emplace<MobHealthBar>(entity);
	auto& healthbar = registry.get<MobHealthBar>(entity);
	healthbar.entity = mob_entity;
	auto& motion = registry.emplace<Motion>(entity);
	auto& mob_motion = registry.get<Motion>(mob_entity);
	motion.position.x = mob_motion.position.x;
	motion.position.y = mob_motion.position.y - abs(mob_motion.scale.y) / 2 - 15;
	motion.angle = 0.f;
	motion.velocity = vec2({ 0, 0 });
	motion.scale = vec2({ std::max(40.f, abs(mob_motion.scale.x) / 2.f), 8.f}); // for boss we may want bigger health bar hence max function
	motion.offset_to_ground = { 0, motion.scale.y / 2.f };
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 1152.f, 648.f };
	sprite.sheet_dims = { 1152.f, 648.f };
	auto& render_request = registry.emplace<RenderRequest>(entity);
	render_request.used_texture = TEXTURE_ASSET_ID::HEALTHBAR_RED;
	render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
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
	motion.scale = GAME_SCALE * vec2(128.f, 128.f);
	motion.offset_to_ground = {0, motion.scale.y / 2.f / 2.5};

	std::cout << "Ship position: " << position.x << ", " << position.y << std::endl;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0.0f, 0.0f};
	sprite.dims = {128.f, 128.f};
    sprite.sheet_dims = {128.f, 128.f};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::SHIP;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity)
{
	std::cout << "projectile created at " << pos.x << " " << pos.y << "\n";
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
	motion.offset_to_ground = {0, motion.scale.y / 2.f};

	auto& hitBox = registry.emplace<HitBox>(entity);
	hitBox.type = HitBoxType::HITBOX_CIRCLE;
	hitBox.shape.circle.radius = motion.scale.x / 2;

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::GOLD_PROJECTILE;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

void createInventory(entt::registry& registry) {
	auto inventory_entity = registry.create();
	auto& inventory = registry.emplace<Inventory>(inventory_entity);
	const float startX = 50.0f;
	const float startY = 50.0f;
	const float SLOT_SIZE = 45.f;
	for (int i = 0; i < MAX_INVENTORY_SLOTS; i++) {
		auto entity = registry.create();
		inventory.slots.push_back(entity);
		registry.emplace<InventorySlot>(entity);
		registry.emplace<UI>(entity);
		registry.emplace<FixedUI>(entity);
		auto& motion = registry.emplace<Motion>(entity);
		motion.angle = 0.0f;
		motion.position = { startX + SLOT_SIZE * i , startY };
		motion.scale = { SLOT_SIZE, SLOT_SIZE };
		motion.velocity = { 0.f, 0.f };
		auto& sprite = registry.emplace<Sprite>(entity);
		sprite.coord = { 0.f, 0.f };
		sprite.dims = { 488.f, 488.f };
		sprite.sheet_dims = { 488.f, 488.f};
		auto& render_request = registry.emplace<RenderRequest>(entity);
		render_request.used_texture = TEXTURE_ASSET_ID::INVENTORY_SLOT;
		render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
		render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	}
}