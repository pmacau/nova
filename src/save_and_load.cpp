#include "common.hpp"
#include <entt.hpp>
#include <string>
#include "tinyECS/components.hpp"
#include "save_and_load.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void SaveAndLoad::save(entt::registry& registry) {
	json registry_json;
	registry_json["entities"] = json::array();
	for (auto entity : registry.view<entt::entity>()) {
		json entity_json;
		if (registry.all_of<Motion>(entity)) {
			auto motion = registry.get<Motion>(entity);
			entity_json["Motion"] = {
				{"position", {{"x", motion.position.x}, {"y", motion.position.y}}},
				{"angle", motion.angle},
				{"velocity", {{"x", motion.velocity.x}, {"y", motion.velocity.y}}},
				{"scale", {{"x", motion.scale.x}, {"y", motion.scale.y}}}
			};
		}
		if (registry.all_of<Player>(entity)) {
			auto player = registry.get<Player>(entity);
			entity_json["Player"] = {
				{"health", player.health}, 
				{"direction", player.direction}
			};
		}
		if (registry.all_of<Mob>(entity)) {
			auto mob = registry.get<Mob>(entity);
			entity_json["Mob"] = {
				{"health", mob.health},
				{"hit_time", mob.hit_time}
			};
		}
		if (registry.all_of<HitBox>(entity)) {
			auto hitbox = registry.get<HitBox>(entity);
			entity_json["HitBox"] = {
				{"x1", hitbox.x1},
				{"x2", hitbox.x2},
				{"x3", hitbox.x3},
				{"y1", hitbox.y1},
				{"y2", hitbox.y2},
			};
		}
		if (registry.all_of<Sprite>(entity)) {
			auto sprite = registry.get<Sprite>(entity);
			entity_json["Sprite"] = {
				{"coord", {{"x", sprite.coord.x}, {"y", sprite.coord.y}}}, 
				{"dims", {{"x", sprite.dims.x}, {"y", sprite.dims.y}}},
				{"sheet_dims", {{"x", sprite.sheet_dims.x}, {"y", sprite.sheet_dims.y}}},
			};
		}
		if (registry.all_of<Animation>(entity)) {
			auto animation = registry.get<Animation>(entity);
			entity_json["Animation"] = {
				{"frame_duration", animation.frameDuration},
				{"frame_time", animation.frameTime}
			};
		}
		if (registry.all_of<RenderRequest>(entity)) {
			auto render_request = registry.get<RenderRequest>(entity);
			entity_json["RenderRequest"] = {
				{"used_texture", static_cast<int>(render_request.used_texture)}, // enum type
				{"used_effect", static_cast<int>(render_request.used_effect)},
				{"used_geometry", static_cast<int>(render_request.used_geometry)}
			};
		}
		if (!entity_json.empty()) { // first one is null for some reason so to prevent that 
			registry_json["entities"].push_back(entity_json);
		}
	}
	std::ofstream file("gamestate.json");
	file << registry_json.dump(4);
	file.close();
}

void SaveAndLoad::load(entt::registry& registry) {
	//registry.clear();
	auto motions = registry.view<Motion>(entt::exclude<Player>);
	registry.destroy(motions.begin(), motions.end());
	std::ifstream file("gamestate.json");
	json registry_json;
	file >> registry_json;
	for (auto entity_json : registry_json["entities"]) {
		auto entity = registry.create();
		if (entity_json.contains("Motion")) {
			Motion motion;
			motion.position = vec2({ entity_json["Motion"]["position"]["x"].get<float>(), entity_json["Motion"]["position"]["y"].get<float>()});
			motion.angle = entity_json["Motion"]["angle"].get<float>();
			motion.velocity = vec2({ entity_json["Motion"]["velocity"]["x"].get<float>(), entity_json["Motion"]["velocity"]["y"].get<float>() });
			motion.scale = vec2({ entity_json["Motion"]["scale"]["x"].get<float>(), entity_json["Motion"]["scale"]["y"].get<float>() });
			registry.emplace<Motion>(entity, motion);
		}
		if (entity_json.contains("Player")) {
			Player player;
			player.health = entity_json["Player"]["health"].get<int>();
			player.direction = entity_json["Player"]["direction"].get<int>();
			registry.emplace<Player>(entity, player);
		}
		if (entity_json.contains("Mob")) {
			Mob mob;
			mob.health  = entity_json["Mob"]["health"].get<int>();
			mob.hit_time = entity_json["Mob"]["hit_time"].get<float>();
			registry.emplace<Mob>(entity, mob);
		}
		if (entity_json.contains("HitBox")) {
			HitBox hitbox;
			hitbox.x1 = entity_json["HitBox"]["x1"].get<float>();
			hitbox.x2 = entity_json["HitBox"]["x2"].get<float>();
			hitbox.x3 = entity_json["HitBox"]["x3"].get<float>();
			hitbox.y1 = entity_json["HitBox"]["y1"].get<float>();
			hitbox.y2 = entity_json["HitBox"]["y2"].get<float>();
			registry.emplace<HitBox>(entity, hitbox);
		}
		if (entity_json.contains("Sprite")) {
			Sprite sprite;
			sprite.coord = vec2({ entity_json["Sprite"]["coord"]["x"].get<float>(), entity_json["Sprite"]["coord"]["y"].get<float>() });
			sprite.dims = vec2({ entity_json["Sprite"]["dims"]["x"].get<float>(), entity_json["Sprite"]["dims"]["y"].get<float>() });
			sprite.sheet_dims = vec2({ entity_json["Sprite"]["sheet_dims"]["x"].get<float>(), entity_json["Sprite"]["sheet_dims"]["y"].get<float>() });
			registry.emplace<Sprite>(entity, sprite);
		}
		if (entity_json.contains("Animation")) {
			Animation animation;
			animation.frameDuration = entity_json["Animation"]["frame_duration"].get<float>();
			animation.frameTime = entity_json["Animation"]["frame_time"].get<float>();
			registry.emplace<Animation>(entity, animation);
		}
		if (entity_json.contains("RenderRequest")) {
			RenderRequest render_request;
			render_request.used_texture = static_cast<TEXTURE_ASSET_ID>(entity_json["RenderRequest"]["used_texture"].get<int>());
			render_request.used_effect = static_cast<EFFECT_ASSET_ID>(entity_json["RenderRequest"]["used_effect"].get<int>());
			render_request.used_geometry = static_cast<GEOMETRY_BUFFER_ID>(entity_json["RenderRequest"]["used_geometry"].get<int>());
			registry.emplace<RenderRequest>(entity, render_request);
		}
	}
}

