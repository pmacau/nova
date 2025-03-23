#include "world_init.hpp"
#include "util/debug.hpp"
#include <iostream>
#include <ai/ai_common.hpp>
#include <ai/ai_component.hpp>
#include "ai/state_machine/ai_state_machine.hpp"
#include "animation/animation_component.hpp"
#include "ai/state_machine/idle_state.hpp"
#include "ai/state_machine/patrol_state.hpp"
#include <ai/ai_initializer.hpp>
#include "collision/hitbox.hpp"
#include <creature/creature_common.hpp>
#include "ui_system.hpp"
#include <map/map_system.hpp>

entt::entity createPlayer(entt::registry& registry, vec2 position)
{
	auto entity = registry.create();

	auto& animComp = registry.emplace<AnimationComponent>(entity);
    animComp.currentAnimationId = "player_idle_down";
    animComp.timer = 0.0f;
    animComp.currentFrameIndex = 0;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = PLAYER_SPRITESHEET.dims;
	sprite.sheet_dims = PLAYER_SPRITESHEET.sheet_dims;

	auto& player = registry.emplace<Player>(entity);
	player.health = PLAYER_HEALTH;
	 
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.formerPosition = position;
	motion.scale = GAME_SCALE * PLAYER_SPRITESHEET.dims;
	motion.offset_to_ground = {0, motion.scale.y / 2.f};

	float w = motion.scale.x;
	float h = motion.scale.y;
	auto& hitbox = registry.emplace<Hitbox>(entity);
	hitbox.pts = {
		{w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
		{w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
	};
	hitbox.depth = 50; // TODO: change this back to unset (epsilon)

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::PLAYER;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createPlayerHealthBar(entt::registry& registry) {
	auto entity = registry.create();
	registry.emplace<FixedUI>(entity);
	registry.emplace<UI>(entity);
	registry.emplace<PlayerHealthBar>(entity);
	auto& motion = registry.emplace<Motion>(entity);
	/*motion.position = { position.x + WINDOW_WIDTH_PX / 2 -  175.f,
						position.y - WINDOW_HEIGHT_PX / 2 + 50.f};*/
	//motion.position = { WINDOW_WIDTH_PX - 175.F, 50.F };
	motion.position = { WINDOW_WIDTH_PX - 175.F, 200.F };
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

entt::entity createMob(entt::registry& registry, vec2 position, int health) {
	// ENTITY CREATION
	auto entity = registry.create();

	auto& mob = registry.emplace<Mob>(entity);
	mob.health = health;
	mob.hit_time = 1.f;
	mob.biome = Mob::Biome::FOREST;
	mob.type = Mob::Type::PURPLE;

	// SPRITE 
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 43.f, 55.f };	
	sprite.sheet_dims = {43.f, 55.f};
	
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	// motion.position = position;
	motion.position.x = position.x;
	motion.position.y = position.y;

	motion.scale = vec2(100, 120);

	// HITBOX
	float w = motion.scale.x;
	float h = motion.scale.y;
	auto& hitbox = registry.emplace<Hitbox>(entity);
	hitbox.pts = {
		{w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
		{w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
	};
	hitbox.depth = 50;

	// motion.scale = vec2(GAME_SCALE * 40.f, GAME_SCALE * 54.f);
	//motion.scale = vec2(38*3, 54*3);
	motion.offset_to_ground = {0, motion.scale.y / 2.f};
	
	UISystem::dropForMob(registry, entity);

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::MOB;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	createMobHealthBar(registry, entity, 15.f);
	// ai set up
	// TODO: create a generic enemy creation
	auto& aiComp = registry.emplace<AIComponent>(entity);
	AIConfig bossConfig = getBossAIConfig();
	const TransitionTable& goblinTransitions = getGoblinTransitionTable();
	aiComp.stateMachine = std::make_unique<AIStateMachine>(registry, entity, bossConfig, goblinTransitions);
   
	aiComp.stateMachine->changeState(g_stateFactory.createState("patrol").release());
	return entity; 
}

entt::entity createMobHealthBar(entt::registry& registry, entt::entity& mob_entity, float y_adjust) {
	auto entity = registry.create();

	registry.emplace<UI>(entity);
	registry.emplace<MobHealthBar>(entity);
	auto& healthbar = registry.get<MobHealthBar>(entity);

	auto& mob = registry.get<Mob>(mob_entity);
	healthbar.entity = mob_entity;
	healthbar.initial_health = mob.health;
	healthbar.y_adjust = y_adjust;

	auto& motion = registry.emplace<Motion>(entity);
	auto& mob_motion = registry.get<Motion>(mob_entity);
	motion.position.x = mob_motion.position.x;
	motion.position.y = mob_motion.position.y - abs(mob_motion.scale.y) / 2 - 15 - healthbar.y_adjust;
	motion.angle = 0.f;
	motion.velocity = vec2({ 0, 0 });
	motion.scale = vec2({ 40.0f, 8.f}); // for boss we may want bigger health bar hence max function
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

entt::entity createMob2(entt::registry& registry, vec2 position, int health) {
	// ENTITY CREATION
	auto entity = registry.create();

	auto& mob = registry.emplace<Mob>(entity);
	mob.health = health;
	mob.hit_time = 1.f;
	mob.biome = Mob::Biome::FOREST;
	mob.type = Mob::Type::TORCH;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = vec2(1344.f / 7, 960.f / 5);
	sprite.sheet_dims = {1344.f, 960.f};

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };

	motion.position.x = position.x;
	motion.position.y = position.y;

	motion.scale = vec2(1344.f / 7, 960.f / 5) * 0.9f;
	motion.offset_to_ground = {0, motion.scale.y / 4.f * 0.8f};

	float w = motion.scale.x * 0.4;
	float h = motion.scale.y * 0.5;
	auto& hitbox = registry.emplace<Hitbox>(entity);
	hitbox.pts = {
		{w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
		{w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
	};
	hitbox.depth = 60;
	
	UISystem::dropForMob(registry, entity);

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	// Setup AnimationComponent (runtime state for animations).
    auto& animComp = registry.emplace<AnimationComponent>(entity);
    animComp.currentAnimationId = "mob2_idle";
    animComp.timer = 0.0f;
    animComp.currentFrameIndex = 0;

	// TODO: create a generic enemy creation
	// set up ai for goblin
	auto& aiComp = registry.emplace<AIComponent>(entity);
	AIConfig goblinConfig = getGoblinAIConfig();
	const TransitionTable& goblinTransitions = getGoblinTransitionTable();
	aiComp.stateMachine = std::make_unique<AIStateMachine>(registry, entity, goblinConfig, goblinTransitions);
   
	aiComp.stateMachine->changeState(g_stateFactory.createState("patrol").release());

	//initial state
	static PatrolState patrolState;
	aiComp.stateMachine->changeState(&patrolState);

	createMobHealthBar(registry, entity, -40.0f);
	return entity; 
}

// GAME PLAY SHIP
entt::entity createShip(entt::registry& registry, vec2 position)
{
	auto entity = registry.create();
	auto& ship = registry.emplace<Ship>(entity);
	ship.health = SHIP_HEALTH;
	ship.range = SHIP_RANGE;
	ship.timer = SHIP_TIMER_S;
	ship.bulletType = Ship::BulletType::GOLD_PROJ;

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 90.0f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * vec2(120, 120);
	motion.offset_to_ground = vec2(0, motion.scale.y / 2);

	float w = motion.scale.x * 0.5;
	float h = motion.scale.y * 0.3;
	auto& hitbox = registry.emplace<Hitbox>(entity);
	hitbox.pts = {
		{w * -0.45f, h * -0.9f}, {w * 0.45f, h * -0.9f},
		{w * 0.45f, h * 0.15f},   {w * -0.45f, h * 0.15f}
	};
	hitbox.depth = 130;

	auto& obstacle = registry.emplace<Obstacle>(entity);
	obstacle.isPassable = false;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
	sprite.dims = {128, 75};
   	sprite.sheet_dims = { 128, 75 };

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::SHIP_VERY_DAMAGE;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createShipWeapon(entt::registry& registry, vec2 position, vec2 size, vec2 sprite_dims, vec2 sprite_sheet_dims, FrameIndex sprite_coords, int weaponNum)
{
	auto entity = registry.create();

	registry.emplace<ShipWeapon>(entity);

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 90.0f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * size;
	motion.offset_to_ground = vec2(0, motion.scale.y / 5);

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = sprite_dims;
	sprite.sheet_dims = sprite_sheet_dims;
	sprite.coord = sprite_coords;

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	weaponNum = std::clamp(weaponNum, 0, static_cast<int>(TEXTURE_ASSET_ID::TEXTURE_COUNT) - 1);

	renderRequest.used_texture = static_cast<TEXTURE_ASSET_ID>(weaponNum);
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createShipEngine(entt::registry& registry, vec2 position, vec2 size, int engineNum)
{
	auto entity = registry.create();

	registry.emplace<ShipEngine>(entity);

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 90.0f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * size;
	motion.offset_to_ground = vec2(0, motion.scale.y / 3);

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
	sprite.dims = {128.f, 128.f};
    sprite.sheet_dims = {128.f, 128.f};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	engineNum = std::clamp(engineNum, 0, static_cast<int>(TEXTURE_ASSET_ID::TEXTURE_COUNT) - 1);

	renderRequest.used_texture = static_cast<TEXTURE_ASSET_ID>(engineNum);
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}


// UI SHIP
entt::entity createUIShip(entt::registry& registry, vec2 position, vec2 size, int shipNum)
{
	auto entity = registry.create();
	registry.emplace<UIShip>(entity);
	registry.emplace<UI>(entity);
	registry.emplace<FixedUI>(entity);

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * size;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
	sprite.dims = {128.f, 128.f};
    sprite.sheet_dims = {128.f, 128.f};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	shipNum = std::clamp(shipNum, 0, static_cast<int>(TEXTURE_ASSET_ID::TEXTURE_COUNT) - 1);

	renderRequest.used_texture = static_cast<TEXTURE_ASSET_ID>(shipNum);
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createUIShipWeapon(entt::registry& registry, vec2 position, vec2 size, vec2 sprite_dims, vec2 sprite_sheet_dims, FrameIndex sprite_coords, int weaponNum)
{
	auto entity = registry.create();
	registry.emplace<UI>(entity);
	registry.emplace<FixedUI>(entity);

	auto& shipWeapon = registry.emplace<UIShipWeapon>(entity);
	shipWeapon.active = false;

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * size;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = sprite_dims;
	sprite.sheet_dims = sprite_sheet_dims;
	sprite.coord = sprite_coords;

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	weaponNum = std::clamp(weaponNum, 0, static_cast<int>(TEXTURE_ASSET_ID::TEXTURE_COUNT) - 1);

	renderRequest.used_texture = static_cast<TEXTURE_ASSET_ID>(weaponNum);
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createUIShipEngine(entt::registry& registry, vec2 position, vec2 size, int engineNum)
{
	auto entity = registry.create();
	registry.emplace<UI>(entity);
	registry.emplace<FixedUI>(entity);

	auto& shipEngine = registry.emplace<UIShipEngine>(entity);
	shipEngine.active = false;

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * size;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
	sprite.dims = {128.f, 128.f};
    sprite.sheet_dims = {128.f, 128.f};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	engineNum = std::clamp(engineNum, 0, static_cast<int>(TEXTURE_ASSET_ID::TEXTURE_COUNT) - 1);

	renderRequest.used_texture = static_cast<TEXTURE_ASSET_ID>(engineNum);
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}



entt::entity createTextBox(entt::registry& registry, vec2 position, vec2 size, std::string text, float scale, vec3 textColor) {
	auto entity = registry.create();

	registry.emplace<UI>(entity);
	registry.emplace<FixedUI>(entity);
	registry.emplace<TextData>(entity, text, scale, textColor);

	auto& motion = registry.emplace<Motion>(entity);
	motion.scale = size;
	motion.position = position;
	motion.velocity = {0.f, 0.f};

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
	sprite.dims = {128.f, 128.f};
    sprite.sheet_dims = {128.f, 128.f};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::TEXTBOX_BACKGROUND;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity, int damage, TEXTURE_ASSET_ID projectileType)
{
	debug_printf(DebugType::WORLD_INIT, "Projectile created: (%.1f, %.1f)\n", pos.x, pos.y);
	auto entity = registry.create();

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = {18.f, 18.f};
	sprite.sheet_dims = {18.f, 18.f};

	auto& projectile = registry.emplace<Projectile>(entity);
	projectile.damage = damage;
	projectile.timer = PROJECTILE_TIMER;

	auto& motion = registry.emplace<Motion>(entity);
	motion.velocity = velocity;
	motion.position = pos;
	motion.scale = size;
	motion.offset_to_ground = {0, motion.scale.y / 2.f};
	motion.angle = atan2(velocity.y, velocity.x) * (180.0f / M_PI) + 90.0f;

	float w = motion.scale.x;
	float h = motion.scale.y;
	auto& hitbox = registry.emplace<Hitbox>(entity);
	hitbox.pts = {
		{w * -0.25f, h * -0.50f}, {w *  0.25f, h * -0.50f}, // Top pts
		{w *  0.50f, h * -0.25f}, {w *  0.50f, h *  0.25f}, // Right pts
		{w *  0.25f, h *  0.50f}, {w * -0.25f, h *  0.50f}, // Bot pts
		{w * -0.50f, h *  0.25f}, {w * -0.50f, h * -0.25f}, // Left pts
	};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = projectileType;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createBoss(entt::registry& registry, vec2 pos) {
	auto entity = createMob(registry, pos, MOB_HEALTH * 10);
	Boss& boss = registry.emplace<Boss>(entity);
	boss.agro_range = 500.f;
	boss.spawn = pos;
	UISystem::dropForMob(registry, entity);

	debug_printf(DebugType::WORLD_INIT, "Boss created at: (%.1f, %.1f)\n", pos.x, pos.y);
	return entity;
}

entt::entity createTree(entt::registry& registry, vec2 pos, FrameIndex spriteCoord) {
	auto entity = registry.create();

	registry.emplace<Tree>(entity);

	auto& motion = registry.emplace<Motion>(entity);
	//motion.scale = GAME_SCALE * vec2(50.f, 99.f);
	//motion.offset_to_ground = GAME_SCALE * vec2(0.f, 49.5f);
	motion.scale = GAME_SCALE * vec2(132.f, 148.f);
	motion.offset_to_ground = GAME_SCALE * vec2(0.f, 74.f);
	motion.position = pos - motion.offset_to_ground;
	motion.velocity = {0.f, 0.f};

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = spriteCoord;
	//sprite.dims = {50.f, 99.f};
	sprite.dims = {132.f, 148.f};
	//sprite.sheet_dims = {250.f, 99.f};
	sprite.sheet_dims = {792.f, 148.f};

	// TODO: make this hitbox trapezoid at the root
	float w = 18.f;
	float h = 16.f;
	float g = 100.f;

	// hitbox is relative to object's center
	auto& hitbox = registry.emplace<Hitbox>(entity);
	hitbox.pts = {
		{w * -0.5f, g + h * -0.5f}, {w * 0.5f, g + h * -0.5f},
		{w * 0.5f, g + h * 0.5f},   {w * -0.5f, g + h * 0.5f}
	};

	auto& obstacle = registry.emplace<Obstacle>(entity);
	obstacle.isPassable = false;

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::TREE;
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
	for (int i = 0; i < 20; i++) {
		auto entity = registry.create();
		inventory.slots.push_back(entity);
		auto& inventory_slot = registry.emplace<InventorySlot>(entity);
		inventory_slot.id = i;
		if (i > 4) {
			registry.emplace<HiddenInventory>(entity);
		}
		registry.emplace<UI>(entity);
		registry.emplace<FixedUI>(entity);
		auto& motion = registry.emplace<Motion>(entity);
		motion.angle = 0.0f;
		motion.position = { startX + SLOT_SIZE * (i % 5) , startY + SLOT_SIZE * (i / 5)};
		motion.scale = { SLOT_SIZE, SLOT_SIZE };
		motion.velocity = { 0.f, 0.f };
		auto& sprite = registry.emplace<Sprite>(entity);
		sprite.coord = { 0, 0 };
		sprite.dims = { 93.f, 95.f };
		sprite.sheet_dims = { 93.f, 95.f};
		auto& render_request = registry.emplace<RenderRequest>(entity);
		render_request.used_texture = TEXTURE_ASSET_ID::INVENTORY_SLOT;
		render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
		render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	}
}

void destroy_creature(entt::registry& registry, entt::entity creature) {
	// find its health bar
	auto view = registry.view<MobHealthBar>();
	for (auto entity : view) {
		auto& healthbar = view.get<MobHealthBar>(entity);
		if (healthbar.entity == creature) {

			if (registry.valid(entity)) {
				registry.destroy(entity);
			}
			break;
		}
	}

	if (registry.valid(creature)) {
		registry.destroy(creature);
	}
}


entt::entity createCreature(entt::registry& registry, vec2 position, CreatureDefinition def, int health)
{
    // ENTITY CREATION
	auto entity = registry.create();

	auto& mob = registry.emplace<Mob>(entity);
	mob.health = health;
	mob.hit_time = 1.f;

	// fix
	mob.biome = Mob::Biome::FOREST;
	mob.type = Mob::Type::TORCH;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = vec2(1344.f / 7, 960.f / 5);
	sprite.sheet_dims = {1344.f, 960.f};

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position.x = position.x + sprite.dims[0] / 2;
	motion.position.y = position.y + sprite.dims[1] / 2;
	motion.scale = vec2(1344.f / 7, 960.f / 5) * 0.9f;
	motion.offset_to_ground = {0, motion.scale.y / 4.f * 0.9f};

	float w = motion.scale.x * 0.4;
	float h = motion.scale.y * 0.5;
	auto& hitbox = registry.emplace<Hitbox>(entity);
	hitbox.pts = {
		{w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
		{w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
	};
	hitbox.depth = 60;
	
	UISystem::dropForMob(registry, entity);

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	// Setup AnimationComponent (runtime state for animations).
    auto& animComp = registry.emplace<AnimationComponent>(entity);
    animComp.currentAnimationId = "mob2_idle";
    animComp.timer = 0.0f;
    animComp.currentFrameIndex = 0;

	// TODO: create a generic enemy creation
	// set up ai for goblin
	auto& aiComp = registry.emplace<AIComponent>(entity);
	AIConfig goblinConfig = getGoblinAIConfig();
	const TransitionTable& goblinTransitions = getGoblinTransitionTable();
	aiComp.stateMachine = std::make_unique<AIStateMachine>(registry, entity, goblinConfig, goblinTransitions);
   
	aiComp.stateMachine->changeState(g_stateFactory.createState("patrol").release());

	//initial state
	static PatrolState patrolState;
	aiComp.stateMachine->changeState(&patrolState);

	createMobHealthBar(registry, entity, -40.0f);
	return entity; 

    return entity;
}

entt::entity createTitleScreen(entt::registry& registry) {
	auto entity = registry.create();
	registry.emplace<UI>(entity);
	registry.emplace<FixedUI>(entity);
	registry.emplace<Title>(entity);
	auto& motion = registry.emplace<Motion>(entity);
	motion.position = { WINDOW_WIDTH_PX / 2.f, WINDOW_HEIGHT_PX / 2.f };
	motion.scale = { WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX};
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = { 0, 0 };
	sprite.dims = { 120.f, 68.f };
	sprite.sheet_dims = { 120.f, 68.f };
	auto& render_request = registry.emplace<RenderRequest>(entity);;
	render_request.used_texture = TEXTURE_ASSET_ID::TITLE;
	render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	// TODO refactor so that each button is a separate texture 
	auto play = registry.create();
	auto& play_option = registry.emplace<TitleOption>(play);
	play_option.type = TitleOption::Option::PLAY;
	play_option.text = "Play"; 
	play_option.position = { 23 * WINDOW_WIDTH_PX / 120.F, 57.5 * WINDOW_HEIGHT_PX / 68.f};
	play_option.size = { 10.0 * WINDOW_WIDTH_PX / 120.f, 11.0f * WINDOW_HEIGHT_PX / 68.f};

	auto exit = registry.create();
	auto& exit_option = registry.emplace<TitleOption>(exit);
	exit_option.type = TitleOption::Option::EXIT;
	exit_option.text = "Exit";
	exit_option.position = { 95.5 * WINDOW_WIDTH_PX / 120.F, 58 * WINDOW_HEIGHT_PX / 68.f };
	exit_option.size = { 9.0 * WINDOW_WIDTH_PX / 120.f, 12.0f * WINDOW_HEIGHT_PX / 68.f };

	auto save = registry.create();
	auto& save_option = registry.emplace<TitleOption>(save);
	save_option.type = TitleOption::Option::SAVE;
	save_option.text = "Save";
	save_option.position = { 48.5 * WINDOW_WIDTH_PX / 120.F, 59.f * WINDOW_HEIGHT_PX / 68.f };
	save_option.size = { 5.0 * WINDOW_WIDTH_PX / 120.f, 8.0f * WINDOW_HEIGHT_PX / 68.f };

	auto load = registry.create();
	auto& restart_option = registry.emplace<TitleOption>(load);
	restart_option.type = TitleOption::Option::RESTART;
	restart_option.text = "Restart";
	restart_option.position = { 109.5 * WINDOW_WIDTH_PX / 120.F, 57.f * WINDOW_HEIGHT_PX / 68.f };
	restart_option.size = { 9.f * WINDOW_WIDTH_PX / 120.f, 10.f * WINDOW_HEIGHT_PX / 68.f };

	auto restart = registry.create();
	auto& load_option = registry.emplace<TitleOption>(restart);
	load_option.type = TitleOption::Option::LOAD;
	load_option.text = "Load";
	load_option.position = { 66.5 * WINDOW_WIDTH_PX / 120.F, 59.f * WINDOW_HEIGHT_PX / 68.f };
	load_option.size = { 5.0 * WINDOW_WIDTH_PX / 120.f, 8.0f * WINDOW_HEIGHT_PX / 68.f };
	return entity;
}

entt::entity createDebugTile(entt::registry& registry, ivec2 tile_indices) {
	auto entity = registry.create();

	registry.emplace<DebugTile>(entity);

	vec2 pos = MapSystem::get_tile_center_pos(tile_indices);

	auto& motion = registry.emplace<Motion>(entity);
	motion.scale = vec2(TILE_SIZE, TILE_SIZE);
	motion.offset_to_ground = GAME_SCALE * vec2(0.f, 49.5f);
	motion.position = pos;
	motion.velocity = {0.f, 0.f};

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {6,7};
	sprite.dims = {TILE_SIZE, TILE_SIZE};
	sprite.sheet_dims = {128.f, 112.f};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::TILESET;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createMinimap(entt::registry & registry) {
	auto entity = registry.create();
	registry.emplace<FixedUI>(entity);
	registry.emplace<UI>(entity);

	auto& motion = registry.emplace<Motion>(entity);
	motion.position = { WINDOW_WIDTH_PX - 175.F, 100.F };
	motion.angle = 0.f;
	motion.velocity = vec2({ 0, 0 });
	motion.scale = vec2(499.f / 3, 499.f / 3);
	motion.offset_to_ground = vec2(0, motion.scale.y / 2.f);

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 499.f, 499.f };
	sprite.sheet_dims = { 499.f, 499.f };

	auto& render_request = registry.emplace<RenderRequest>(entity);
	render_request.used_texture = TEXTURE_ASSET_ID::MINIMAP;
	render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	return entity;
}

entt::entity createButton(entt::registry& registry, vec2 position, vec2 size, ButtonOption::Option option, std::string text)
{
	auto entity = registry.create();
	registry.emplace<UI>(entity);
	registry.emplace<FixedUI>(entity);
	registry.emplace<Button>(entity);

	auto& current_option = registry.emplace<ButtonOption>(entity);
	current_option.type = option;
	current_option.text = text;
	current_option.position = position;
	// current_option.size = GAME_SCALE * vec2(120.f / scale.x, 128.f / scale.y);
	current_option.size = GAME_SCALE * size;


	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * size;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
	sprite.dims = {128.f, 128.f};
    sprite.sheet_dims = {128.f, 128.f};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::SELECTION_BUTTON;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createUpgradeButton(entt::registry& registry, vec2 position, vec2 size, ButtonOption::Option option, TEXTURE_ASSET_ID buttonID)
{
	auto entity = registry.create();
	registry.emplace<UI>(entity);
	registry.emplace<FixedUI>(entity);
	auto& upgradeButton = registry.emplace<UpgradeButton>(entity);
	upgradeButton.text = "Upgrade";

	auto& current_option = registry.emplace<ButtonOption>(entity);
	current_option.type = option;
	// current_option.text = text;
	current_option.position = position;
	current_option.size = GAME_SCALE * size;

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * size;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
	sprite.dims = {128.f, 128.f};
    sprite.sheet_dims = {128.f, 128.f};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = buttonID;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createIcon(entt::registry& registry, vec2 position, vec2 scale, int iconNum, vec2 sprite_dims, vec2 sprite_sheet_dims)
{
	auto entity = registry.create();
	registry.emplace<UIIcon>(entity);
	registry.emplace<UI>(entity);
	registry.emplace<FixedUI>(entity);

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * scale;

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
	sprite.dims = sprite_dims;
	sprite.sheet_dims = sprite_sheet_dims;

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	iconNum = std::clamp(iconNum, 0, static_cast<int>(TEXTURE_ASSET_ID::TEXTURE_COUNT) - 1);

	renderRequest.used_texture = static_cast<TEXTURE_ASSET_ID>(iconNum);
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}