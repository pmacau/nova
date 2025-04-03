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


	auto& dash = registry.emplace<Dash>(entity); 
	dash.cooldown = 3.0f;
	dash.remainingDuration = 0.0f;

	

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
	auto entity_outer = registry.create();
	registry.emplace<FixedUI>(entity_outer);
	registry.emplace<UI>(entity_outer);
	auto& motion_outer = registry.emplace<Motion>(entity_outer);
	motion_outer.position = { WINDOW_WIDTH_PX - 175.F, 50.F };
	motion_outer.angle = 0.f;
	motion_outer.velocity = vec2({ 0, 0 });
	motion_outer.scale = vec2({ 270.f, 30.f });
	motion_outer.offset_to_ground = vec2(0, motion_outer.scale.y / 2.f);
	auto& sprite_outer = registry.emplace<Sprite>(entity_outer);
	sprite_outer.dims = { 256.f, 36.f };
	sprite_outer.sheet_dims = { 256.f, 36.f };
	auto& render_request_outer = registry.emplace<RenderRequest>(entity_outer);
	render_request_outer.used_texture = TEXTURE_ASSET_ID::PLAYER_HEALTH_OUTER;
	render_request_outer.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request_outer.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	auto entity = registry.create();
	registry.emplace<FixedUI>(entity);
	registry.emplace<UI>(entity);
	registry.emplace<PlayerHealthBar>(entity);
	auto& motion = registry.emplace<Motion>(entity);
	motion.position = { WINDOW_WIDTH_PX - 175.F, 50.F };
	motion.angle = 0.f;
	motion.velocity = vec2({ 0, 0 });
	motion.scale = vec2({ 250.f, 15.f });
	motion.offset_to_ground = vec2(0, motion.scale.y / 2.f);
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 210.f, 12.f };
	sprite.sheet_dims = { 210.f, 12.f };
	auto& render_request = registry.emplace<RenderRequest>(entity);
	render_request.used_texture = TEXTURE_ASSET_ID::PLAYER_HEALTH_INNER;
	render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	return entity;
}


//Motion& slashOffSetHelper(entt::registry& registry) {
//	auto player = registry.view<Player>().front(); 
//	auto& direction = registry.get<InputState>(player); 
//	std::cout << direction.down << " " << direction.up << " " << direction.left << " " << direction.right << std::endl; 
//	auto& motion = registry.get<Motion>(player); 
//	return motion; 
//}

entt::entity createSlash(entt::registry& registry) {  
   auto entity = registry.create();  

   registry.emplace<Slash>(entity);  
   auto player = registry.view<Player>().front();

   auto motion_player = registry.get<Motion>(player); 

   auto& motion = registry.emplace<Motion>(entity);  
   motion.angle = 0.f;  
   motion.velocity = {0, 0};  
   motion.position = motion_player.position;  
   motion.scale = motion.scale * 10.f;  // change later to a more acceptable value 
   motion.offset_to_ground = {0, 0};  

   float w = motion.scale.x;
   float h = motion.scale.y;
   auto& hitbox = registry.emplace<Hitbox>(entity);
   hitbox.pts = {
	   {w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
	   {w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
   };
   // have an animation animation (for another day)
   /*auto& renderRequest = registry.emplace<RenderRequest>(entity);  
   renderRequest.used_texture = TEXTURE_ASSET_ID::SLASH;  
   renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;  
   renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;  */

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

entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity, int damage, int timer, TEXTURE_ASSET_ID projectileType)
{
	debug_printf(DebugType::WORLD_INIT, "Projectile created: (%.1f, %.1f)\n", pos.x, pos.y);
	auto entity = registry.create();

	auto& sprite = registry.emplace<Sprite>(entity);
	if (projectileType == TEXTURE_ASSET_ID::GOLD_PROJECTILE) {
		sprite.dims = { 18.f, 18.f };
		sprite.sheet_dims = { 18.f, 18.f };
	}
	else if (projectileType == TEXTURE_ASSET_ID::MISSILE_PROJECTILE) {
		sprite.dims = { 56.f, 156.f };
		sprite.sheet_dims = { 56.f, 156.f };
	}
	else if (projectileType == TEXTURE_ASSET_ID::SHOTGUN_PROJECTILE) {
		sprite.dims = { 512.f, 512.f };
		sprite.sheet_dims = { 512.f, 512.f };
	}
	else {
		// SHOULD DO FOR EACH PROJECTILE TYPE
		sprite.dims = { 18.f, 18.f };
		sprite.sheet_dims = { 18.f, 18.f };
	}

	auto& projectile = registry.emplace<Projectile>(entity);
	projectile.damage = damage;
	projectile.timer = timer;

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


std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<float> flip(0.f, 1.f);
std::uniform_int_distribution<int> variation(0, 2);

void setTreeType(
	entt::registry& registry,
	entt::entity entity,
	vec2 pos,
	Biome biome, Terrain terrain
) {
	vec2 box_dims = {132.f, 148.f};

	auto& motion = registry.emplace<Motion>(entity);
	motion.scale = GAME_SCALE * box_dims;
	motion.offset_to_ground = GAME_SCALE * vec2(0.f, box_dims.y / 2.f);
	motion.position = pos - motion.offset_to_ground;
	motion.velocity = {0.f, 0.f};

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.sheet_dims = {1320.f, 296.f};
	sprite.dims = box_dims;

	auto& hitbox = registry.emplace<Hitbox>(entity);
	float w = 18.f, h = 16.f, g = 100.f;

	bool normal = flip(rng) <= 0.7;
	int t = variation(rng);

	switch (biome) {
		case B_JUNGLE:
			if (normal) {
				sprite.coord = {1, 0 + t};
			} else {
				w = 24.f;
				sprite.coord = {1, 3 + t};
			}
			break;

		case B_ICE:
			if (normal) {
				sprite.coord = {0, 3};
			} else {
				sprite.coord = {0, 4};
			}
			break;

		case B_SAVANNA:
			if (normal) {
				hitbox.depth = 20.f;
				sprite.coord = {0, 7 + t};
			} else {
				w = 38.f;
				sprite.coord = {1, 7 + t};
			}
			break;

		case B_BEACH:
			sprite.coord = {0, 5};
			break;

		default: // set to forest stats otherwise
			sprite.coord = {0, 0 + t};
			break;
	}

	if (terrain == Terrain::SAND) {
		sprite.coord = {0, 6};
		w = 18.f;
	}

	// hitbox is relative to object's center
	hitbox.pts = {
		{w * -0.5f, g + h * -0.5f}, {w * 0.5f, g + h * -0.5f},
		{w * 0.5f, g + h * 0.5f},   {w * -0.5f, g + h * 0.5f}
	};
}

entt::entity createTree(entt::registry& registry, vec2 pos, Biome biome, Terrain terrain) {
	auto entity = registry.create();
	registry.emplace<Tree>(entity);

	auto& obstacle = registry.emplace<Obstacle>(entity);
	obstacle.isPassable = false;

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::TREE;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	setTreeType(registry, entity, pos, biome, terrain);

	return entity;
}

void createInventory(entt::registry& registry) {
	auto inventory_entity = registry.create();
	auto& inventory = registry.emplace<Inventory>(inventory_entity);
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
		motion.position = { 50.f + 45.f * (i % 5) , 50.f + 45.f * (i / 5) };
		motion.scale = { 45.f, 45.f };
		motion.velocity = { 0.f, 0.f };
		auto& sprite = registry.emplace<Sprite>(entity);
		sprite.coord = { 0, 0 };
		sprite.dims = { 93.f, 95.f };
		sprite.sheet_dims = { 93.f, 95.f };
		auto& render_request = registry.emplace<RenderRequest>(entity);
		render_request.used_texture = TEXTURE_ASSET_ID::INVENTORY_SLOT;
		render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
		render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	}
}

void createDefaultWeapon(entt::registry& registry) {
	auto& slot_entity = registry.get<Inventory>(*registry.view<Inventory>().begin()).slots[0];
	auto& inventory_slot = registry.get<InventorySlot>(slot_entity);
	auto default_weapon_entity = registry.create();
	registry.emplace<UI>(default_weapon_entity);
	registry.emplace<FixedUI>(default_weapon_entity);
	auto& motion = registry.emplace<Motion>(default_weapon_entity);
	motion.position = { 50.f, 50.f };
	motion.scale = vec2(121.f, 54.f) / 4.0f;
	auto& item = registry.emplace<Item>(default_weapon_entity);
	item.type = Item::Type::DEFAULT_WEAPON;
	inventory_slot.hasItem = true;
	inventory_slot.item = default_weapon_entity;
	auto& sprite = registry.emplace<Sprite>(default_weapon_entity);
	sprite.dims = { 121.f, 54.f };
	sprite.sheet_dims = { 121.f, 54.f };
	auto& render_request_weapon = registry.emplace<RenderRequest>(default_weapon_entity);
	render_request_weapon.used_texture = TEXTURE_ASSET_ID::DEFAULT_WEAPON;
	render_request_weapon.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request_weapon.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	registry.emplace<ActiveSlot>(slot_entity);
	auto& render_request = registry.get<RenderRequest>(slot_entity);
	render_request.used_texture = TEXTURE_ASSET_ID::INVENTORY_SLOT_ACTIVE;
	// TODO the weapon upgrade should be available from ship
	auto& slot_entity_2 = registry.get<Inventory>(*registry.view<Inventory>().begin()).slots[1];
	auto& inventory_slot_2 = registry.get<InventorySlot>(slot_entity_2);
	auto homing_missile_entity = registry.create();
	registry.emplace<UI>(homing_missile_entity);
	registry.emplace<FixedUI>(homing_missile_entity);
	auto& missile_motion = registry.emplace<Motion>(homing_missile_entity);
	missile_motion.position = { 50.f + 45.f, 50.f };
	missile_motion.scale = vec2(700.0f, 400.0f) / 21.0f;
	auto& missile_item = registry.emplace<Item>(homing_missile_entity);
	missile_item.type = Item::Type::HOMING_MISSILE;
	inventory_slot_2.hasItem = true;
	inventory_slot_2.item = homing_missile_entity;
	auto& missile_sprite = registry.emplace<Sprite>(homing_missile_entity);
	missile_sprite.dims = { 700.0f, 400.0f };
	missile_sprite.sheet_dims = { 700.0f, 400.0f };
	auto& render_request_missile = registry.emplace<RenderRequest>(homing_missile_entity);
	render_request_missile.used_texture = TEXTURE_ASSET_ID::HOMING_MISSILE;
	render_request_missile.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request_missile.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	auto& slot_entity_3 = registry.get<Inventory>(*registry.view<Inventory>().begin()).slots[2];
	auto& inventory_slot_3 = registry.get<InventorySlot>(slot_entity_3);
	auto shotgun_entity = registry.create();
	registry.emplace<UI>(shotgun_entity);
	registry.emplace<FixedUI>(shotgun_entity);
	auto& shotgun_motion = registry.emplace<Motion>(shotgun_entity);
	shotgun_motion.position = { 50.f + 45.f * 2, 50.f };
	shotgun_motion.scale = vec2(512.f, 512.f) / 15.0f;
	auto& shotgun_item = registry.emplace<Item>(shotgun_entity);
	shotgun_item.type = Item::Type::SHOTGUN;
	inventory_slot_3.hasItem = true;
	inventory_slot_3.item = shotgun_entity;
	auto& shotgun_sprite = registry.emplace<Sprite>(shotgun_entity);
	shotgun_sprite.dims = { 512.f, 512.f };
	shotgun_sprite.sheet_dims = { 512.f, 512.f };
	auto& render_request_shotgun = registry.emplace<RenderRequest>(shotgun_entity);
	render_request_shotgun.used_texture = TEXTURE_ASSET_ID::SHOTGUN;
	render_request_shotgun.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request_shotgun.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
}

void findNearestTarget(entt::registry& registry, entt::entity& entity, float x, float y) {
	float radius = 10.0f;
	while (radius <= WINDOW_WIDTH_PX / 2) {
		bool found = false;
		for (auto mob_entity : registry.view<Mob>()) {
			auto& motion = registry.get<Motion>(mob_entity);
			if (abs(motion.position.x - x) <= radius && abs(motion.position.y - y) <= radius) {
				registry.emplace<HomingMissile>(entity).target = mob_entity;
				found = true;
				break;
			}
		}
		if (found) break;
		radius += 10.f;
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
	motion.position = { WINDOW_WIDTH_PX - 175.F, 150.F };
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