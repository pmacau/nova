#include "world_init.hpp"
// #include "tinyECS/registry.hpp"
#include <iostream>

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! TODO A1: implement grid lines as gridLines with renderRequests and colors
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
entt::entity createGridLine(entt::registry& registry, vec2 start_pos, vec2 end_pos)
{
	auto entity = registry.create();

	// TODO A1: create a gridLine component

	// re-use the "DEBUG_LINE" renderRequest
	/*
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::EGG,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
		}
	);
	*/

	// TODO A1: grid line color (choose your own color)

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

entt::entity createTower(entt::registry& registry, vec2 position)
{
	auto entity = registry.create();

	// new tower
	auto& t = registry.emplace<Tower>(entity);
	t.range = (float)WINDOW_WIDTH_PX / (float)GRID_CELL_WIDTH_PX;
	t.timer_ms = TOWER_TIMER_MS;	// arbitrary for now

	// Initialize the motion
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 180.f;	// A1-TD: CK: rotate to the left 180 degrees to fix orientation
	motion.velocity = { 0.0f, 0.0f };
	motion.position = position;

	std::cout << "INFO: tower position: " << position.x << ", " << position.y << std::endl;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -TOWER_BB_WIDTH, TOWER_BB_HEIGHT });

	// create an (empty) Tower component to be able to refer to all towers
	registry.emplace<Deadly>(entity);

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::TOWER;
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;

	return entity;
}

void removeTower(entt::registry& registry, vec2 position) {
	// remove any towers at this position
	auto view = registry.view<Tower, Motion>();
	for (auto entity: view) {
		const auto& motion = registry.get<Motion>(entity);
		if (motion.position.y == position.y) {
			registry.destroy(entity);
			std::cout << "tower removed" << std::endl;
		}
	}

	// for (Entity& tower_entity : registry.towers.entities) {
	// 	// get each tower's position to determine it's row
	// 	const Motion& tower_motion = registry.motions.get(tower_entity);
		
	// 	if (tower_motion.position.y == position.y) {
	// 		// remove this tower
	// 		registry.remove_all_components_of(tower_entity);
			
	// 	}
	//}
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

entt::entity createLine(entt::registry& registry, vec2 position, vec2 scale)
{
	auto entity = registry.create();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	// registry.renderRequests.insert(
	// 	entity,
	// 	{
	// 		// usage TEXTURE_COUNT when no texture is needed, i.e., an .obj or other vertices are used instead
	// 		TEXTURE_ASSET_ID::TEXTURE_COUNT,
	// 		EFFECT_ASSET_ID::EGG,
	// 		GEOMETRY_BUFFER_ID::DEBUG_LINE
	// 	}
	// );

	auto& renderRequest = registry.emplace<RenderRequest>(entity);
	renderRequest.used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	renderRequest.used_effect = EFFECT_ASSET_ID::EGG;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::DEBUG_LINE;

	// Create motion
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.emplace<DebugComponent>(entity);
	return entity;
}