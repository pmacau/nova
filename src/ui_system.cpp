#include "tinyECS/components.hpp"
#include "ui_system.hpp"

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