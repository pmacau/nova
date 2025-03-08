#include "world_init.hpp"
#include "util/debug.hpp"
#include <iostream>
#include "ai/ai_common.hpp"
#include "ai/ai_component.hpp"
#include "ai/state_machine/ai_state_machine.hpp"
#include "animation/animation_component.hpp"


entt::entity createPlayer(entt::registry& registry, vec2 position)
{
	auto entity = registry.create();

	auto& animComp = registry.emplace<AnimationComponent>(entity);
    animComp.currentAnimationId = "player_idle_down";
    animComp.timer = 0.0f;
    animComp.currentFrameIndex = 0;


	auto& sprite = registry.emplace<Sprite>(entity);
	// sprite.dims = PLAYER_SPRITESHEET.dims;
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

	// HITBOX
	auto& hitBox = registry.emplace<HitBox>(entity); 
	hitBox.type = HitBoxType::HITBOX_CIRCLE; 
	hitBox.shape.circle.radius = 40.f; 
	/*hitBox.type = HitBoxType::HITBOX_RECT;
	hitBox.shape.rect.width = 43.f;
	hitBox.shape.rect.height = 55.f;*/


	
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	// motion.position = position;

	motion.position.x = position.x + sprite.dims[0] / 2;
	motion.position.y = position.y + sprite.dims[1] / 2;
	motion.scale = vec2(100, 120);

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

	createMobHealthBar(registry, entity);
	return entity; 
}

entt::entity createMobHealthBar(entt::registry& registry, entt::entity& mob_entity) {
	auto entity = registry.create();

	registry.emplace<UI>(entity);
	registry.emplace<MobHealthBar>(entity);
	auto& healthbar = registry.get<MobHealthBar>(entity);

	auto& mob = registry.get<Mob>(mob_entity);
	healthbar.entity = mob_entity;
	healthbar.initial_health = mob.health;

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

entt::entity createMob2(entt::registry& registry, vec2 position, int health) {
	// ENTITY CREATION
	auto entity = registry.create();

	auto& mob = registry.emplace<Mob>(entity);
	mob.health = health;
	mob.hit_time = 1.f;

	// SPRITE 
	auto& sprite = registry.emplace<Sprite>(entity);
	// sprite.dims = { 43.f, 55.f };	
	sprite.dims = vec2(1344.f / 7, 960.f / 5);

	sprite.sheet_dims = {1344.f, 960.f};

	// HITBOX
	auto& hitBox = registry.emplace<HitBox>(entity); 
	hitBox.type = HitBoxType::HITBOX_CIRCLE; 
	hitBox.shape.circle.radius = 30.f; 
	/*hitBox.type = HitBoxType::HITBOX_RECT;
	hitBox.shape.rect.width = 43.f;
	hitBox.shape.rect.height = 55.f;*/
	
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	// motion.position = position;

	motion.position.x = position.x + sprite.dims[0] / 2;
	motion.position.y = position.y + sprite.dims[1] / 2;
	motion.scale = vec2(1344.f / 7, 960.f / 5) * 0.9f;

	// motion.scale = vec2(GAME_SCALE * 40.f, GAME_SCALE * 54.f);
	//motion.scale = vec2(38*3, 54*3);
	motion.offset_to_ground = {0, motion.scale.y / 4.f * 0.9f};

	registry.emplace<Eatable>(entity);
	
	auto& drop = registry.emplace<Drop>(entity);
	drop.item_type = ITEM_TYPE::POTION;

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	// Setup AnimationComponent (runtime state for animations).
    auto& animComp = registry.emplace<AnimationComponent>(entity);
    animComp.currentAnimationId = "mob2_idle";
    animComp.timer = 0.0f;
    animComp.currentFrameIndex = 0;

	createMobHealthBar(registry, entity);
	return entity; 
}



//entt::entity createRockType1(entt::registry& registry, vec2 position) {
//	auto entity = registry.create();
//	auto& sprite = registry.emplace<Sprite>(entity);
//	sprite.dims = { 5.f, 5.f };
//	sprite.sheet_dims = { 144.f, 135.f };
//	auto& hitBox = registry.emplace<HitBox>(entity);
//	hitBox.type = HitBoxType::HITBOX_RECT;
//	hitBox.shape.rect.width = 54.f;
//	hitBox.shape.rect.height = 54.f;
//	/*hitBox.type = HitBoxType::HITBOX_CIRCLE;
//	hitBox.shape.circle.radius = 12.f;*/
//	auto& motion = registry.emplace<Motion>(entity);
//	motion.angle = 0.f;
//	motion.velocity = { 0, 0 };
//	motion.position = position;
//	motion.scale = vec2(54, 54);
//	auto& renderRequest = registry.emplace<RenderRequest>(entity);
//	auto& obstacle = registry.emplace<Obstacle>(entity);
//	obstacle.isPassable = false;
//
//	renderRequest.used_texture = TEXTURE_ASSET_ID::STONE_BLOCK_1;
//	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
//	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
//	return entity;
//
//}


//entt::entity createTreeType1(entt::registry& registry, vec2 position) {
//	auto entity = registry.create();
//	auto& sprite = registry.emplace<Sprite>(entity);
//	sprite.dims = { 80.f, 80.f };
//	sprite.sheet_dims = { 800.f, 944.f };
//	auto& hitBox = registry.emplace<HitBox>(entity);
//	/*hitBox.type = HitBoxType::HITBOX_RECT;
//	hitBox.shape.rect.width = 54.f;
//	hitBox.shape.rect.height = 54.f;*/
//	hitBox.type = HitBoxType::HITBOX_CIRCLE;
//	hitBox.shape.circle.radius = 115.f;
//	auto& motion = registry.emplace<Motion>(entity);
//	motion.angle = 0.f;
//	motion.velocity = { 0, 0 };
//	motion.position = position;
//	motion.scale = vec2(250, 250);
//	auto& renderRequest = registry.emplace<RenderRequest>(entity);
//	auto& obstacle = registry.emplace<Obstacle>(entity);
//	obstacle.isPassable = false;
//
//	renderRequest.used_texture = TEXTURE_ASSET_ID::TREE;
//	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
//	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
//	return entity;
//
//}

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

	debug_printf(DebugType::WORLD_INIT, "Ship position (%d, %d)\n", position.x, position.y);

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
    // sprite.dims = {19 * 15, 35 * 7};
	sprite.dims = {128, 75};
   sprite.sheet_dims = { 128, 75 };

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::SHIP6;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createUIShip(entt::registry& registry, vec2 position, vec2 scale, int shipNum)
{

	auto entity = registry.create();
	registry.emplace<UIShip>(entity);
	registry.emplace<UI>(entity);
	registry.emplace<FixedUI>(entity);

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = GAME_SCALE * vec2(120.f / scale.x, 128.f / scale.y);
	// motion.offset_to_ground = {0, motion.scale.y / 2.f / 2.5};

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = {0, 0};
	sprite.dims = {128.f, 128.f};
    sprite.sheet_dims = {128.f, 128.f};

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	shipNum = std::clamp(shipNum, 1, 6);
	renderRequest.used_texture = static_cast<TEXTURE_ASSET_ID>(static_cast<int>(TEXTURE_ASSET_ID::SHIP1) + (shipNum - 1));
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

entt::entity createProjectile(entt::registry& registry, vec2 pos, vec2 size, vec2 velocity)
{
	debug_printf(DebugType::WORLD_INIT, "Projectile created: (%.1f, %.1f)\n", pos.x, pos.y);
	auto entity = registry.create();

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = {18.f, 18.f};
	sprite.sheet_dims = {18.f, 18.f};

	auto& projectile = registry.emplace<Projectile>(entity);
	projectile.damage = PROJECTILE_DAMAGE;
	projectile.timer = PROJECTILE_TIMER;

	auto& motion = registry.emplace<Motion>(entity);
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

entt::entity createBoss(entt::registry& registry, vec2 pos) {
	auto entity = createMob(registry, pos, MOB_HEALTH * 10);
	Boss& boss = registry.emplace<Boss>(entity);
	boss.agro_range = 500.f;
	boss.spawn = pos;

	debug_printf(DebugType::WORLD_INIT, "Boss created at: (%.1f, %.1f)\n", pos.x, pos.y);
	return entity;
}

entt::entity createTree(entt::registry& registry, vec2 pos, FrameIndex spriteCoord) {
	auto entity = registry.create();

	registry.emplace<Tree>(entity);

	auto& motion = registry.emplace<Motion>(entity);
	motion.scale = GAME_SCALE * vec2(50.f, 99.f);
	motion.offset_to_ground = GAME_SCALE * vec2(0.f, 49.5f);
	motion.position = pos - motion.offset_to_ground;
	motion.velocity = {0.f, 0.f};

	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.coord = spriteCoord;
	sprite.dims = {50.f, 99.f};
	sprite.sheet_dims = {100.f, 99.f};

	// TODO: allow for more flexible hitboxes. I want to be able to "walk through"
	//       the leaves (from behind), but I want a hitbox on the trunk

	// auto& hitBox = registry.emplace<HitBox>(entity);
	// hitBox.type = HitBoxType::HITBOX_RECT;
	// hitBox.shape.rect.height = GAME_SCALE * 50.f;
	// hitBox.shape.rect.width = GAME_SCALE * 10.f;

	// auto& obstacle = registry.emplace<Obstacle>(entity);
	// obstacle.isPassable = false;

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
		sprite.coord = { 0, 0 };
		sprite.dims = { 488.f, 488.f };
		sprite.sheet_dims = { 488.f, 488.f};
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
			registry.destroy(entity);
			break;
		}
	}

	registry.destroy(creature);
}


entt::entity createCreature(entt::registry& registry, vec2 position, CreatureType creatureType, int health)
{
    // Create the entity.
    auto entity = registry.create();

    // --- Motion Component ---
    auto& motion = registry.emplace<Motion>(entity);
    // Set the position (adjusted to center the sprite if desired)
    motion.position = position;
    motion.angle = 0.f;
    motion.velocity = {0, 0};
    
    // Choose scale based on creature type.
    if (creatureType == CreatureType::Boss) {
        motion.scale = vec2(200, 240);  // Example boss scale
    } else {
        motion.scale = vec2(100, 120);  // Default scale for Mob and Mutual
    }
    motion.offset_to_ground = {0, motion.scale.y / 2.f};

    // --- Sprite Component ---
    auto& sprite = registry.emplace<Sprite>(entity);
    if (creatureType == CreatureType::Boss) {
        sprite.dims = {80.f, 110.f};
        sprite.sheet_dims = {80.f, 110.f};
    } else if (creatureType == CreatureType::Mutual) {
        sprite.dims = {40.f, 50.f};
        sprite.sheet_dims = {40.f, 50.f};
    } else { // Mob
        sprite.dims = {43.f, 55.f};
        sprite.sheet_dims = {43.f, 55.f};
    }
    // Optionally set sprite.coord for initial frame.

    // --- HitBox Component ---
    auto& hitBox = registry.emplace<HitBox>(entity);
    hitBox.type = HITBOX_CIRCLE;
    if (creatureType == CreatureType::Boss) {
        hitBox.shape.circle.radius = 60.f;  // Example value for boss
    } else {
        hitBox.shape.circle.radius = 40.f;
    }

    // --- Creature-Specific Component ---
    if (creatureType == CreatureType::Mob || creatureType == CreatureType::Mutual) {
        auto& mob = registry.emplace<Mob>(entity);
        mob.health = health;
        mob.hit_time = 1.f;
    } else if (creatureType == CreatureType::Boss) {
        auto& boss = registry.emplace<Boss>(entity);
        boss.agro_range = 500.f;  // Example value
        boss.spawn = position;
        // Optionally set boss-specific health here.
    }

    // --- AI Component ---
    // Attach our AI state machine to control creature behavior.
    auto& aiComp = registry.emplace<AIComponent>(entity);
    aiComp.stateMachine = std::make_unique<AIStateMachine>(registry, entity);
    // Set initial state to Idle. (Using a static instance for now; can later be created per entity if needed.)
    // static IdleState idleState;
    // aiComp.stateMachine->changeState(&idleState);

    // // --- Render Request Component ---
    // auto& renderRequest = registry.emplace<RenderRequest>(entity);
    // switch (creatureType) {
    //     case CreatureType::Mob:
    //         renderRequest.used_texture = TEXTURE_ASSET_ID::MOB;
    //         break;
    //     case CreatureType::Boss:
    //         renderRequest.used_texture = TEXTURE_ASSET_ID::SHIP; // Example: Boss uses a different texture
    //         break;
    //     case CreatureType::Mutual:
    //         renderRequest.used_texture = TEXTURE_ASSET_ID::TREE; // Example: Mutual creatures might use tree texture
    //         break;
    // }
    // renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
    // renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

    // Optionally, attach additional components like Animation if needed later.

    // (Optional) Create associated UI, health bars, etc.
    // For example: createMobHealthBar(registry, entity);

    return entity;
}