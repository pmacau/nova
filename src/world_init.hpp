#pragma once

#include "common.hpp"
#include <entt.hpp>
#include "render_system.hpp"
#include "ai/ai_common.hpp"
#include <creature/creature_common.hpp>
#include <creature/creature_defs/creature_definition_data.hpp>


entt::entity createPlayer(entt::registry& registry, vec2 position);

entt::entity createPlayerHealthBar(entt::registry& registry);

entt::entity createShip(entt::registry& registry, vec2 position);
entt::entity createShipWeapon(entt::registry& registry, vec2 position, vec2 size, vec2 sprite_dims, vec2 sprite_sheet_dims, FrameIndex sprite_coords, int weaponNum);
entt::entity createShipEngine(entt::registry& registry, vec2 position, vec2 size, int engineNum);

entt::entity createUIShip(entt::registry& registry, vec2 position, vec2 scale, int shipNum);
entt::entity createUIShipWeapon(entt::registry& registry, vec2 position, vec2 size, vec2 sprite_dims, vec2 sprite_sheet_dims, FrameIndex sprite_coords, int weaponNum);
entt::entity createUIShipEngine(entt::registry& registry, vec2 position, vec2 size, int engineNum);

// invaders
// entt::entity createMob(entt::registry& registry, vec2 position, int health = MOB_HEALTH);
// entt::entity createMob2(entt::registry& registry, vec2 position, int health = MOB_HEALTH);

entt::entity createMobHealthBar(entt::registry& registry, entt::entity& mob_entity, float y_adjust);

// projectile
entt::entity createProjectile(
    entt::registry& registry, 
    vec2 pos, 
    vec2 size, 
    vec2 velocity, 
    int damage, 
    int timer, 
    TEXTURE_ASSET_ID projectileType,
    std::vector<ColliderType> targetTypes = { ColliderType::CREATURE });

entt::entity createSlash(entt::registry& registry); 

// entt::entity createBoss(entt::registry& registry, vec2 pos);
entt::entity createTree(entt::registry& registry, vec2 pos, Biome biome, Terrain terrain);
entt::entity createHouse(entt::registry& registry, vec2 pos, Biome biome);

entt::entity createTextBox(entt::registry& registry, vec2 position, vec2 size, std::string text, float scale, vec3 textColor);
entt::entity createButton(entt::registry& registry, vec2 position, vec2 size, ButtonOption::Option option, std::string text, TEXTURE_ASSET_ID buttonID, ScreenState::ScreenType screenType);
entt::entity createUpgradeButton(entt::registry& registry, vec2 position, vec2 size, ButtonOption::Option option, TEXTURE_ASSET_ID buttonID, ScreenState::ScreenType screenType, std::string text);
entt::entity createIcon(entt::registry& registry, vec2 position, vec2 scale, TEXTURE_ASSET_ID icon, vec2 sprite_dims, vec2 sprite_sheet_dims, ScreenState::ScreenType screenType);


// terrain
// entt::entity createRockType1(entt::registry& registry, vec2 position);

// entt::entity createTreeType1(entt::registry& registry, vec2 position);

// camera
entt::entity createCamera(entt::registry& registry, entt::entity target);

void createInventory(entt::registry& registry);

void destroy_creature(entt::registry& registry, entt::entity creature);

entt::entity createCreature(entt::registry& registry, vec2 position, const CreatureDefinitionData& def, int health);

entt::entity createTitleScreen(entt::registry & registry);

entt::entity createMinimap(entt::registry & registry);

entt::entity createDebugTile(entt::registry& registry, ivec2 tile_indices);

entt::entity createDefaultWeapon(entt::registry& registry);
entt::entity createHomingMissleWeapon(entt::registry& registry);
entt::entity createShotgunWeapon(entt::registry& registry);

void findNearestTarget(entt::registry& registry, entt::entity& entity, float x, float y);