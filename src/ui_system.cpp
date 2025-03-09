#include "tinyECS/components.hpp"
#include "ui_system.hpp"
#include <iostream>
#include "util/debug.hpp"


void UISystem::updatePlayerHealthBar(entt::registry& registry, int health) {
	for (auto entity : registry.view<PlayerHealthBar>()) {
		auto& playerhealth_motion = registry.get<Motion>(entity);
		float left = playerhealth_motion.position.x - playerhealth_motion.scale.x / 2.f;
		playerhealth_motion.scale = vec2({ health * (250.f / PLAYER_HEALTH), 15.f });
		playerhealth_motion.position.x = left + playerhealth_motion.scale.x / 2.f;
		break; 
	}
}

void UISystem::updateMobHealthBar(entt::registry& registry, entt::entity& mob_entity, bool hit) {
	auto& mob = registry.get<Mob>(mob_entity);
	auto& mob_motion = registry.get<Motion>(mob_entity);
	for (auto entity : registry.view<MobHealthBar>()) {
		auto& healthbar = registry.get<MobHealthBar>(entity);
		if (healthbar.entity == mob_entity) {
			auto& mobhealth_motion = registry.get<Motion>(entity);
			if (hit) {
				float left_adjust = abs(mobhealth_motion.scale.x) / 2.f;
				mobhealth_motion.scale = vec2({ mob.health * 40.f / healthbar.initial_health, 8.f });
				healthbar.left_adjust += left_adjust - abs(mobhealth_motion.scale.x) / 2.f;
			}
			mobhealth_motion.position.x = mob_motion.position.x - healthbar.left_adjust;
			mobhealth_motion.position.y = mob_motion.position.y - abs(mob_motion.scale.y) / 2.f - healthbar.y_adjust;
			break;
		}
	}
}

void UISystem::renderItem(entt::registry& registry, entt::entity& mob_entity) {
	if (registry.all_of<Drop>(mob_entity)) {
		auto& drop = registry.get<Drop>(mob_entity);
		auto entity = registry.create();
		if (drop.item_type == ITEM_TYPE::POTION) {
			auto& item = registry.emplace<Item>(entity);
			item.item_type = drop.item_type;
			auto& potion = registry.emplace<Potion>(entity);
			potion.heal = 20;
			auto& mob_motion = registry.get<Motion>(mob_entity);
			auto& motion = registry.emplace<Motion>(entity);
			motion.angle = 0.f;
			motion.position = mob_motion.position;
			motion.scale = vec2(512.f, 508.f) / 15.f;
			motion.velocity = { 0.f, 0.f };
			auto& sprite = registry.emplace<Sprite>(entity);
			sprite.coord = { 0, 0 };
			sprite.dims = { 512.f, 508.f };
			sprite.sheet_dims = { 512.f, 508.f };
			auto& render_request = registry.emplace<RenderRequest>(entity);
			render_request.used_texture = TEXTURE_ASSET_ID::POTION;
			render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
			render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
		}
	}
}

void UISystem::useItem(entt::registry& registry, entt::entity& entity) {
	auto& item = registry.get<Item>(entity);
	if (item.item_type == ITEM_TYPE::POTION) {
		auto& potion = registry.get<Potion>(entity);
		for (auto player_entity : registry.view<Player>()) {
			auto& player = registry.get<Player>(player_entity);
			player.health = min(player.health + potion.heal, PLAYER_HEALTH);
			MusicSystem::playSoundEffect(SFX::POTION);
			updatePlayerHealthBar(registry, player.health);	
			auto screens = registry.view<ScreenState>();
			auto& screen = registry.get<ScreenState>(screens.front());
			if (screens.size() > 0) {
				screen.darken_screen_factor = std::max(screen.darken_screen_factor - 0.33f, 0.0f);
			}
			break;
		}
	}
}


bool UISystem::useItemFromInventory(entt::registry& registry, float mouse_pos_x, float mouse_pos_y) {
	auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
	if (mouse_pos_y >= 50.f - 45.f / 2.f && mouse_pos_y <= 50.f + 45.f / 2.f && mouse_pos_x >= 50.f - 45.f / 2.f) {
		int i = (int)((mouse_pos_x - (50.f - 45.f / 2.f)) / 45.f);
		if (i >= 0 && i < inventory.slots.size()) {
			auto& inventory_entity = inventory.slots[i];
			auto& inventory_slot = registry.get<InventorySlot>(inventory_entity);
			if (inventory_slot.hasItem) {
				useItem(registry, inventory_slot.item);
				inventory_slot.hasItem = false;

				debug_printf(DebugType::PHYSICS, "Destroying entity (ui sys)\n");
				registry.destroy(inventory_slot.item);
				return true;
			}
		}
	}
	return false;
}

void UISystem::addToInventory(entt::registry& registry, entt::entity& item_entity) {
	auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
	for (int i = 0; i < inventory.slots.size(); i++) {
		auto& inventory_slot = registry.get<InventorySlot>(inventory.slots[i]);
		if (!inventory_slot.hasItem) {
			inventory_slot.hasItem = true;
			inventory_slot.item = item_entity;
			registry.emplace<UI>(item_entity);
			registry.emplace<FixedUI>(item_entity);
			auto& motion = registry.get<Motion>(item_entity);
			motion.position = { 50.f + 45.f * i, 50.f };

			MusicSystem::playSoundEffect(SFX::PICKUP);
			break;
		}
	}
}

void UISystem::equipItem(entt::registry& registry, Motion& player_motion) {
	for (auto entity : registry.view<Motion, Item>()) {
		auto& motion = registry.get<Motion>(entity);
		if (abs(player_motion.position.x - motion.position.x) <= 20 && abs(player_motion.position.y - motion.position.y) <= 20) {
			addToInventory(registry, entity);
			break;
		}
	}
}