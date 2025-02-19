#include "tinyECS/components.hpp"
#include "ui_system.hpp"
#include <iostream>

void UISystem::updateMobHealthBarMotion(entt::registry& registry, vec2 direction) {
	for (auto entity : registry.view<MobHealthBar>()) {
		auto& motion = registry.get<Motion>(entity);
		motion.velocity = direction;
	}
}

void UISystem::updatePlayerHealthBar(entt::registry& registry, int health) {
	for (auto entity : registry.view<PlayerHealthBar>()) {
		auto& playerhealth_motion = registry.get<Motion>(entity);
		float left = playerhealth_motion.position.x - playerhealth_motion.scale.x;
		playerhealth_motion.scale = vec2({ health * (120.f / PLAYER_HEALTH), 8});
		playerhealth_motion.position.x = left + playerhealth_motion.scale.x; // to keep the healthbar fixed to left because scaling happens relative to center
		break; // since there is only one healthbar, is there a better way instead of doing the loop?
	}
}

void UISystem::updateMobHealthBar(entt::registry& registry, entt::entity& mob_entity) {
	auto& mob = registry.get<Mob>(mob_entity);
	auto& mob_motion = registry.get<Motion>(mob_entity);
	for (auto entity : registry.view<MobHealthBar>()) {
		auto& healthbar = registry.get<MobHealthBar>(entity);
		if (healthbar.ent == mob_entity) {
			auto& mobhealth_motion = registry.get<Motion>(entity);
			float left = mobhealth_motion.position.x - mobhealth_motion.scale.x;
			mobhealth_motion.scale = vec2({ mob.health * std::max(30.f, mob_motion.scale.x / 3) / MOB_HEALTH, 5 }); 
			mobhealth_motion.position.x = left + mobhealth_motion.scale.x;
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
			motion.scale = {265.f / (265.f + 401.f) * 20, 401.f / (265.f + 401.f) * 20};
			motion.velocity = {0.f, 0.f};
			auto& sprite = registry.emplace<Sprite>(entity);
			sprite.coord = {56.f / 401.f, 124.f / 265.f};
			sprite.dims = {265.f, 401.f};
			sprite.sheet_dims = {512.f, 508.f};
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
			updatePlayerHealthBar(registry, player.health);
			break;
		}
	}
}


void UISystem::useItemFromInventory(entt::registry& registry, float mouse_pos_x, float mouse_pos_y) {
	auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
	if (mouse_pos_y >= 25.f && mouse_pos_y <= 75.f && mouse_pos_x >= 25.f) {
		int i = (int)((mouse_pos_x - 25.f) / 50.f);
		if (i >= 0 && i < inventory.slots.size()) {
			auto& inventory_entity = inventory.slots[i];
			auto& inventory_slot = registry.get<InventorySlot>(inventory_entity);
			if (inventory_slot.hasItem) {
				useItem(registry, inventory_slot.item);
				inventory_slot.hasItem = false;
				registry.destroy(inventory_slot.item);
			}
		}
	}
}

void UISystem::addToInventory(entt::registry& registry, entt::entity& item_entity) {
	auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
	for (int i = 0; i < inventory.slots.size(); i++) {
		auto& inventory_slot = registry.get<InventorySlot>(inventory.slots[i]);
		if (!inventory_slot.hasItem) {
			inventory_slot.hasItem = true;
			inventory_slot.item = item_entity;
			auto& motion = registry.get<Motion>(item_entity);
			motion.position = {50.f + 48.f * i, 50.f};
			break;
		}
	}
}

void UISystem::equipItem(entt::registry& registry, Motion& player_motion) {
	for (auto entity : registry.view<Motion, Item>()) {
		auto& motion = registry.get<Motion>(entity);
		if (abs(player_motion.position.x - motion.position.x) <= 10 && abs(player_motion.position.y - motion.position.y) <= 10) {
			addToInventory(registry, entity);
			break;
		}
	}
}