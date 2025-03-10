#include "tinyECS/components.hpp"
#include "ui_system.hpp"
#include <iostream>
#include "util/debug.hpp"

float UISystem::equip_delay = 0.0f;

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

entt::entity UISystem::renderItemAtPos(entt::registry& registry, entt::entity item_to_copy_entity, float x, float y, bool ui) {
	auto entity = registry.create();
	auto& item_to_copy = registry.get<Item>(item_to_copy_entity);
	if (ui) {
		registry.emplace<UI>(entity);
		registry.emplace<FixedUI>(entity);
	}
	if (item_to_copy.item_type == ITEM_TYPE::POTION) {
		auto& item = registry.emplace<Item>(entity);
		item.item_type = item_to_copy.item_type;
		if (!ui) {
			item.no = item_to_copy.no;
		}
		auto& potion = registry.emplace<Potion>(entity);
		potion.heal = 20;
		auto& motion = registry.emplace<Motion>(entity);
		motion.angle = 0.f;
		motion.position = { x, y };
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
	return entity;
}

void UISystem::dropItem(entt::registry& registry, Click click) {
	if (!registry.view<Drag>().empty()) {
		auto& drag_entity = *registry.view<Drag>().begin();
		auto& player_entity = *registry.view<Player>().begin();
		auto& motion = registry.get<Motion>(player_entity);
		auto& item = registry.get<Item>(drag_entity);
		if (click == Click::CTRLLEFT) {
			renderItemAtPos(registry, drag_entity, motion.position.x, motion.position.y, false);
			item.no = 0;
		}
		else if (click == Click::SHIFTLEFT) {
			int item_no = item.no;
			item.no = std::max(1, item.no / 2);
			renderItemAtPos(registry, drag_entity, motion.position.x, motion.position.y, false);
			item.no = item_no - std::max(1, item_no / 2);
		}
		else if (click == Click::ALTLEFT) {
			int item_no = item.no;
			item.no = std::min(item.no, 5);
			renderItemAtPos(registry, drag_entity, motion.position.x, motion.position.y, false);
			item.no = item_no - std::min(item_no, 5);
		}
		else {
			int item_no = item.no;
			item.no = 1;
			renderItemAtPos(registry, drag_entity, motion.position.x, motion.position.y, false);
			item.no = item_no - 1;
		}
		if (item.no == 0) {
			registry.destroy(drag_entity);
		}
		MusicSystem::playSoundEffect(SFX::PICKUP); // TODO change it to drop
	}
}

void UISystem::resetDragItem(entt::registry& registry) {
	auto& drag_entity = *registry.view<Drag>().begin();
	auto& inventory_slot = registry.get<InventorySlot>(registry.get<Drag>(drag_entity).slot);
	if (inventory_slot.hasItem) {
		auto& inventory_item = registry.get<Item>(inventory_slot.item);
		inventory_item.no += registry.get<Item>(drag_entity).no;
	}
	else {
		auto item = renderItemAtPos(registry, drag_entity, 50.f + 45.f * inventory_slot.id, 50.f, true);
		inventory_slot.hasItem = true;
		inventory_slot.item = item;
		auto& inventory_item = registry.get<Item>(inventory_slot.item);
		inventory_item.no = registry.get<Item>(drag_entity).no;
	}
	registry.destroy(drag_entity);
}

void UISystem::updateDragItem(entt::registry& registry, float mouse_pos_x, float mouse_pos_y) {
	auto& entity = *registry.view<Drag>().begin();
	auto& motion = registry.get<Motion>(entity);
	motion.position = { mouse_pos_x, mouse_pos_y };
}


// what's single responsibilty principle :|
bool UISystem::useItemFromInventory(entt::registry& registry, float mouse_pos_x, float mouse_pos_y, Click click) {
	auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
	if (mouse_pos_y >= 50.f - 45.f / 2.f && mouse_pos_y <= 50.f + 45.f / 2.f && mouse_pos_x >= 50.f - 45.f / 2.f) {
		int i = (int)((mouse_pos_x - (50.f - 45.f / 2.f)) / 45.f);
		if (i >= 0 && i < inventory.slots.size()) {
			auto& inventory_entity = inventory.slots[i];
			auto& inventory_slot = registry.get<InventorySlot>(inventory_entity);
			if (inventory_slot.hasItem) {
				auto& inventory_item = registry.get<Item>(inventory_slot.item);
				// left mouse click
				if (click == Click::LEFT || click == Click::CTRLLEFT || click == Click::SHIFTLEFT || click == Click::ALTLEFT) {
					// use item if no item is being dragged
					if (registry.view<Drag>().empty()) {
						useItem(registry, inventory_slot.item);
						if (inventory_item.no == 1) {
							inventory_item.no = 0;
							inventory_slot.hasItem = false;
							debug_printf(DebugType::PHYSICS, "Destroying entity (ui sys)\n");
							registry.destroy(inventory_slot.item);
						}
						else {
							inventory_item.no -= 1;
						}
					}
					else {
						auto& drag_entity = *registry.view<Drag>().begin();
						auto& drag_item = registry.get<Item>(drag_entity);
						// add dragged item to inventory slot if of the same type
						if (registry.get<Item>(inventory_slot.item).item_type == drag_item.item_type) {
							if (click == Click::CTRLLEFT) {
								inventory_item.no += drag_item.no;
								drag_item.no = 0;
							}
							else if (click == Click::SHIFTLEFT) {
								inventory_item.no += std::max(1, drag_item.no / 2);
								drag_item.no -= std::max(1, drag_item.no / 2);
							}
							else if (click == Click::ALTLEFT) {
								inventory_item.no += std::min(drag_item.no, 5);
								drag_item.no -= std::min(drag_item.no, 5);
							}
							else {
								inventory_item.no += 1;
								drag_item.no -= 1;
							}
							if (drag_item.no == 0) {
								registry.destroy(drag_entity);
							}
						}
						// put the dragged item to original inventory slot if the item is of different type
						else {
							resetDragItem(registry);
						}
					}
				}
				// right mouse click
				else {
					// create item at mouse position if no item is being dragged
					if (registry.view<Drag>().empty()) {
						auto item_entity_on_mouse = renderItemAtPos(registry, inventory_slot.item, mouse_pos_x, mouse_pos_y, true);
						auto& drag = registry.emplace<Drag>(item_entity_on_mouse);
						drag.slot = inventory_entity;
						auto& item = registry.get<Item>(item_entity_on_mouse);
						if (click == Click::CTRLRIGHT) {
							item.no = inventory_item.no;
						}
						else if (click == Click::SHIFTRIGHT) {
							item.no = std::max(1, inventory_item.no / 2);
						}
						else if (click == Click::ALTRIGHT) {
							item.no = std::min(inventory_item.no, 5);
						}
					}
					else {
						auto& item_on_mouse = registry.get<Item>(*registry.view<Drag>().begin());
						// increase the number of items currently being dragged if same type item is picked
						if (item_on_mouse.item_type == registry.get<Item>(inventory_slot.item).item_type) {
							if (click == Click::CTRLRIGHT) {
								item_on_mouse.no += inventory_item.no;
							}
							else if (click == Click::SHIFTRIGHT) {
								item_on_mouse.no += std::max(1, inventory_item.no / 2);
							}
							else if (click == Click::ALTRIGHT) {
								item_on_mouse.no += std::min(inventory_item.no, 5);
							}
							else {
								item_on_mouse.no += 1;
							}
						}
						// put the held item back to original inventory slot and drag a new item
						else {
							resetDragItem(registry);
							auto item_entity_on_mouse = renderItemAtPos(registry, inventory_slot.item, mouse_pos_x, mouse_pos_y, true);
							auto& drag = registry.emplace<Drag>(item_entity_on_mouse);
							drag.slot = inventory_entity;
							auto& item = registry.get<Item>(item_entity_on_mouse);
							if (click == Click::CTRLRIGHT) {
								item.no = inventory_item.no;
							}
							else if (click == Click::SHIFTRIGHT) {
								item.no = std::max(1, inventory_item.no / 2);
							}
							else if (click == Click::ALTRIGHT) {
								item.no = std::min(inventory_item.no, 5);
							}
						}
					}
					if (click == Click::CTRLRIGHT) {
						inventory_item.no = 0;
						inventory_slot.hasItem = false;
						debug_printf(DebugType::PHYSICS, "Destroying entity (ui sys)\n");
						registry.destroy(inventory_slot.item);
					}
					else if (click == Click::SHIFTRIGHT) {
						if (inventory_item.no == 1) {
							inventory_item.no = 0;
							inventory_slot.hasItem = false;
							debug_printf(DebugType::PHYSICS, "Destroying entity (ui sys)\n");
							registry.destroy(inventory_slot.item);
						}
						else {
							inventory_item.no -= std::max(1, inventory_item.no / 2);
						}
					}
					else if (click == Click::ALTRIGHT) {
						if (inventory_item.no <= 5) {
							inventory_item.no = 0;
							inventory_slot.hasItem = false;
							debug_printf(DebugType::PHYSICS, "Destroying entity (ui sys)\n");
							registry.destroy(inventory_slot.item);
						}
						else {
							inventory_item.no -= std::min(inventory_item.no, 5);
						}
					}
					else {
						if (inventory_item.no == 1) {
							inventory_item.no = 0;
							inventory_slot.hasItem = false;
							debug_printf(DebugType::PHYSICS, "Destroying entity (ui sys)\n");
							registry.destroy(inventory_slot.item);
						}
						else {
							inventory_item.no -= 1;
						}
					}
				}
			}
			else {
				// place drag item at the empty inventory slot
				if (!registry.view<Drag>().empty()) {
					auto& drag_entity = *registry.view<Drag>().begin();
					auto& drag_item = registry.get<Item>(drag_entity);
					auto item = renderItemAtPos(registry, drag_entity, 50.f + 45.f * i, 50.f, true);
					inventory_slot.hasItem = true;
					inventory_slot.item = item;
					auto& inventory_item = registry.get<Item>(inventory_slot.item);
					if (click == Click::CTRLLEFT || click == Click::CTRLRIGHT) {
						inventory_item.no = drag_item.no;
						drag_item.no = 0;
					}
					else if (click == Click::SHIFTLEFT || click == Click::SHIFTRIGHT) {
						inventory_item.no = std::max(1, drag_item.no / 2);
						drag_item.no -= std::max(1, drag_item.no / 2);
					}
					else if (click == Click::ALTLEFT || click == Click::ALTRIGHT) {
						inventory_item.no = std::min(drag_item.no, 5);
						drag_item.no -= std::min(drag_item.no, 5);
					}
					else {
						inventory_item.no = 1;
						drag_item.no -= 1;
					}
					if (drag_item.no == 0) {
						registry.destroy(drag_entity);
					}
				}
			}
			return true;
		}
	}
	return false;
}

void UISystem::addToInventory(entt::registry& registry, entt::entity& item_entity) {
	auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
	auto& item = registry.get<Item>(item_entity);
	// check for existing slots having same item type
	for (int i = 0; i < inventory.slots.size(); i++) {
		auto& inventory_slot = registry.get<InventorySlot>(inventory.slots[i]);
		if (inventory_slot.hasItem) {
			auto& inventory_item = registry.get<Item>(inventory_slot.item);
			if (inventory_item.item_type == item.item_type) {
				inventory_item.no += item.no;
				MusicSystem::playSoundEffect(SFX::PICKUP);
				registry.destroy(item_entity);
				return;
			}
		}
	}
	// add item to empty slot
	for (int i = 0; i < inventory.slots.size(); i++) {
		auto& inventory_slot = registry.get<InventorySlot>(inventory.slots[i]);
		if (!inventory_slot.hasItem) {
			inventory_slot.hasItem = true;
			inventory_slot.item = item_entity;
			auto& inventory_item = registry.get<Item>(inventory_slot.item);
			inventory_item.no = item.no;
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
	if (equip_delay > 2.f) {
		for (auto entity : registry.view<Motion, Item>()) {
			auto& motion = registry.get<Motion>(entity);
			if (abs(player_motion.position.x - motion.position.x) <= 20 && abs(player_motion.position.y - motion.position.y) <= 20) {
				addToInventory(registry, entity);
				break;
			}
		}
	}
}
