#pragma once
#include <entt.hpp>

class UISystem {
    public:
        static void updateMobHealthBarMotion(entt::registry& registry, vec2 direction);
        static void updatePlayerHealthBar(entt::registry& registry, int health);
        static void updateMobHealthBar(entt::registry& registry, entt::entity& mob_entity);
        static void renderItem(entt::registry& registry, entt::entity& mob_entity);
        static void equipItem(entt::registry& registry, Motion& player_motion);
        static void addToInventory(entt::registry& registry, entt::entity& item_entity);
        static void useItemFromInventory(entt::registry& registry, float mouse_pos_x, float mouse_pos_y);
        static void useItem(entt::registry& registry, entt::entity& entity);
};