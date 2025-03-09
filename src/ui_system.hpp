#pragma once
#include <entt.hpp>
#include "music_system.hpp"

class UISystem {
	public:
		static void updatePlayerHealthBar(entt::registry& registry, int health);
		static void updateMobHealthBar(entt::registry& registry, entt::entity& mob_entity, bool hit);
		static void renderItem(entt::registry& registry, entt::entity& mob_entity);
		static void equipItem(entt::registry& registry, Motion& player_motion);
		static void addToInventory(entt::registry& registry, entt::entity& item_entity);
		static bool useItemFromInventory(entt::registry& registry, float mouse_pos_x, float mouse_pos_y, bool pick);
		static void useItem(entt::registry& registry, entt::entity& entity);
		static entt::entity renderItemAtPos(entt::registry& registry, entt::entity item_to_copy_entity, float mouse_pos_x, float mouse_pos_y);
		static void updateDragItem(entt::registry& registry, float mouse_pos_x, float mouse_pos_y);
		static void resetDragItem(entt::registry& registry);
};