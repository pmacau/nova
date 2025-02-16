#pragma once
#include <entt.hpp>

class UISystem {
    public:
        static void updateMobHealthBarMotion(entt::registry& registry, vec2 direction);
        static void updatePlayerHealthBar(entt::registry& registry, int health);
        static void updateMobHealthBar(entt::registry& registry, entt::entity& mob_entity);
};