#pragma once
#include <entt.hpp>
#include "music_system.hpp"
#include <random>

class UISystem {
public:
	static float equip_delay;
	static void updatePlayerHealthBar(entt::registry& registry, int health);
	static void updateMobHealthBar(entt::registry& registry, entt::entity& mob_entity, bool hit);
	//static void renderItem(entt::registry& registry, entt::entity& mob_entity);
	static void equipItem(entt::registry& registry, Motion& player_motion);
	static void addToInventory(entt::registry& registry, entt::entity& item_entity);
	static bool useItemFromInventory(entt::registry& registry, float mouse_pos_x, float mouse_pos_y, Click click);
	static void useItem(entt::registry& registry, entt::entity& inventory_slot_entity, float mouse_pos_x, float mouse_pos_y);
	static entt::entity renderItemAtPos(entt::registry& registry, entt::entity item_to_copy_entity, float mouse_pos_x, float mouse_pos_y, bool ui, bool drop);
	static void updateDragItem(entt::registry& registry, float mouse_pos_x, float mouse_pos_y);
	static void resetDragItem(entt::registry& registry);
	static void dropItem(entt::registry& registry);
	static void clearInventoryAndDrop(entt::registry& registry, float x, float y);
	static void clearInventory(entt::registry& registry);
	static void dropForMob(entt::registry& registry, entt::entity& entity);
	static void mobDrop(entt::registry& registry, entt::entity& mob_entity);
private:
	static bool log_inventory;
	static std::mt19937 rng;
	static std::uniform_real_distribution<float> uniform_dist;
	static void removeActiveSlot(entt::registry& registry, entt::entity& inventory_slot_entity);
	static void logItem(entt::registry& registry, Item item);
	static void logInventory(entt::registry& registry);
};