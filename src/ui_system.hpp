#pragma once
#include <entt.hpp>

class UISystem {
    public:
        static void updateMobHealthBarMotion(entt::registry& registry, vec2 direction);
        static void updatePlayerHealthBar(entt::registry& registry, int health);
        static void updateMobHealthBar(entt::registry& registry, entt::entity& mob_entity);
        static void renderItem(entt::registry& registry, entt::entity& mob_entity);
        static bool equipItem(entt::registry& registry, std::vector<entt::entity> inventory, float mouse_pos_x, float mouse_pos_y);
        static void addToInventory(entt::registry& registry, std::vector<entt::entity> inventory, entt::entity& entity);
        static bool useItemFromInventory(entt::registry& registry, std::vector<entt::entity> inventory, float mouse_pos_x, float mouse_pos_y);
        static void useItem(entt::registry& registry, entt::entity& entity, InventorySlot& inventory_entity);
};