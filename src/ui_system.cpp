#include "tinyECS/components.hpp"
#include "ui_system.hpp"
#include <iostream>
#include "util/debug.hpp"

float UISystem::equip_delay = 0.0f;
std::mt19937 UISystem::rng(std::random_device{}());
std::uniform_real_distribution<float> UISystem::uniform_dist(0.f, 1.f);

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
			mobhealth_motion.position = computeHealthBarPosition(mob_motion, { healthbar.left_adjust, healthbar.y_adjust });
			break;
		}
	}
}

void UISystem::useItem(entt::registry& registry, entt::entity& inventory_slot_entity, float mouse_pos_x, float mouse_pos_y) {
	auto& inventory_slot = registry.get<InventorySlot>(inventory_slot_entity);
	auto& entity = inventory_slot.item;
	auto& item = registry.get<Item>(entity);
	if (item.type == Item::Type::POTION) {
		auto& potion = registry.get<Potion>(entity);
		auto& player = registry.get<Player>(*registry.view<Player>().begin());
		std::cout << "player health before: " << player.health << "\n";
		player.health = min(player.health + potion.heal, PLAYER_HEALTH);
		std::cout << "player health after: " << player.health << "\n";
		MusicSystem::playSoundEffect(SFX::POTION);
		updatePlayerHealthBar(registry, player.health);
		auto screens = registry.view<ScreenState>();
		auto& screen = registry.get<ScreenState>(screens.front());
		if (screens.size() > 0) {
			screen.darken_screen_factor = std::max(screen.darken_screen_factor - 0.33f, 0.0f);
		}
		if (item.no == 1) {
			item.no = 0;
			inventory_slot.hasItem = false;
			registry.destroy(entity);
		}
		else {
			item.no -= 1;
		}
	}
	else if (item.type == Item::Type::DEFAULT_WEAPON ||
		item.type == Item::Type::HOMING_MISSILE ||
		item.type == Item::Type::SHOTGUN) {
		for (auto entity : registry.view<ActiveSlot>()) {
			removeActiveSlot(registry, entity);
			break;
		}
		registry.emplace<ActiveSlot>(inventory_slot_entity);
		auto& render_request = registry.get<RenderRequest>(inventory_slot_entity);
		render_request.used_texture = TEXTURE_ASSET_ID::INVENTORY_SLOT_ACTIVE;
		auto& player = registry.get<Player>(*registry.view<Player>().begin());
	}
}

void UISystem::mobDrop(entt::registry& registry, entt::entity& mob_entity) {
	auto& drop = registry.get<Drop>(mob_entity);
	auto& motion = registry.get<Motion>(mob_entity);
	int i = 0;
	auto& items = drop.items;
	while (!items.empty()) {
		if (i == 0) {
			renderItemAtPos(registry, mob_entity, motion.position.x, motion.position.y, false, true);
		}
		else if (i % 2 == 0) {
			renderItemAtPos(registry, mob_entity, motion.position.x - 30.f, motion.position.y, false, true);
		}
		else {
			renderItemAtPos(registry, mob_entity, motion.position.x + 30.f, motion.position.y, false, true);
		}
		items.erase(items.begin());
		i++;
	}
}

entt::entity UISystem::renderItemAtPos(entt::registry& registry, entt::entity item_to_copy_entity, float x, float y, bool ui, bool drop) {
	auto entity = registry.create();
	auto& item_to_copy = drop ? registry.get<Drop>(item_to_copy_entity).items.front() : registry.get<Item>(item_to_copy_entity);
	auto& item = registry.emplace<Item>(entity);
	item.type = item_to_copy.type;
	if (ui) {
		registry.emplace<UI>(entity);
		registry.emplace<FixedUI>(entity);
		if (y > 50.f + 45.f / 2.f && !registry.view<HiddenInventory>().empty()) {
			registry.emplace<HiddenInventory>(entity);
		}
	}
	else {
		item.no = item_to_copy.no;
	}
	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.position = { x, y };
	motion.velocity = { 0.f, 0.f };
	auto& sprite = registry.emplace<Sprite>(entity);
	auto& render_request = registry.emplace<RenderRequest>(entity);
	render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	if (item_to_copy.type == Item::Type::POTION) {
		if (registry.any_of<Potion>(item_to_copy_entity)) {
			auto& potion = registry.get<Potion>(item_to_copy_entity);
			auto& potion_copy = registry.emplace<Potion>(entity);
			potion_copy.heal = potion.heal;
		}
		else {
			auto& potion = registry.emplace<Potion>(entity);
			potion.heal = 20;
		}
		motion.scale = vec2(512.f, 512.f) / 15.f;
		sprite.dims = { 512.f, 512.f };
		sprite.sheet_dims = { 512.f, 512.f };
		render_request.used_texture = TEXTURE_ASSET_ID::POTION;
	}
	else if (item_to_copy.type == Item::Type::IRON) {
		motion.scale = vec2(512.f, 512.f) / 15.f;
		sprite.dims = { 512.f, 512.f };
		sprite.sheet_dims = { 512.f, 512.f };
		render_request.used_texture = TEXTURE_ASSET_ID::IRON;
	}
	else if (item_to_copy.type == Item::Type::COPPER) {
		motion.scale = vec2(512.f, 512.f) / 15.f;
		sprite.dims = { 512.f, 512.f };
		sprite.sheet_dims = { 512.f, 512.f };
		render_request.used_texture = TEXTURE_ASSET_ID::COPPER;
	}
	else if (item_to_copy.type == Item::Type::DEFAULT_WEAPON) {
		motion.scale = vec2(121.f, 54.f) / 4.0f;
		sprite.dims = { 121.f, 54.f };
		sprite.sheet_dims = { 121.f, 54.f };
		render_request.used_texture = TEXTURE_ASSET_ID::DEFAULT_WEAPON;
	}
	else if (item_to_copy.type == Item::Type::HOMING_MISSILE) {
		motion.scale = vec2(700.0f, 400.0f) / 21.0f;
		sprite.dims = { 700.0f, 400.0f };
		sprite.sheet_dims = { 700.0f, 400.0f };
		render_request.used_texture = TEXTURE_ASSET_ID::HOMING_MISSILE;
	}
	else if (item_to_copy.type == Item::Type::SHOTGUN) {
		motion.scale = vec2(512.f, 512.f) / 15.f;
		sprite.dims = { 512.f, 512.f };
		sprite.sheet_dims = { 512.f, 512.f };
		render_request.used_texture = TEXTURE_ASSET_ID::SHOTGUN;
	}
	return entity;
}

void UISystem::dropItem(entt::registry& registry) {
	if (!registry.view<Drag>().empty()) {
		auto& drag_entity = *registry.view<Drag>().begin();
		auto& player_entity = *registry.view<Player>().begin();
		auto& motion = registry.get<Motion>(player_entity);
		renderItemAtPos(registry, drag_entity, motion.position.x, motion.position.y, false, false);
		registry.destroy(drag_entity);
		MusicSystem::playSoundEffect(SFX::DROP);
	}
}

void UISystem::resetDragItem(entt::registry& registry) {
	auto& drag_entity = *registry.view<Drag>().begin();
	auto& drag = registry.get<Drag>(drag_entity);
	if (drag.noSlot) {
		for (auto entity : registry.get<Inventory>(*registry.view<Inventory>().begin()).slots) {
			auto& inventory_slot = registry.get<InventorySlot>(entity);
			if (!inventory_slot.hasItem) {
				auto item = renderItemAtPos(registry, drag_entity, 50.f + 45.f * (inventory_slot.id % 5), 50.f + 45.f * (inventory_slot.id / 5), true, false);
				if (!registry.view<HiddenInventory>().empty() && inventory_slot.id > 4) {
					registry.emplace<HiddenInventory>(item);
				}
				inventory_slot.hasItem = true;
				inventory_slot.item = item;
				auto& inventory_item = registry.get<Item>(inventory_slot.item);
				inventory_item.no = registry.get<Item>(drag_entity).no;
				break;
			}
		}
	}
	else {
		auto& inventory_slot = registry.get<InventorySlot>(drag.slot);
		if (inventory_slot.hasItem) {
			auto& inventory_item = registry.get<Item>(inventory_slot.item);
			inventory_item.no += registry.get<Item>(drag_entity).no;
		}
		else {
			auto item = renderItemAtPos(registry, drag_entity, 50.f + 45.f * (inventory_slot.id % 5), 50.f + 45.f * (inventory_slot.id / 5), true, false);
			if (!registry.view<HiddenInventory>().empty() && inventory_slot.id > 4) {
				registry.emplace<HiddenInventory>(item);
			}
			inventory_slot.hasItem = true;
			inventory_slot.item = item;
			auto& inventory_item = registry.get<Item>(inventory_slot.item);
			inventory_item.no = registry.get<Item>(drag_entity).no;
		}
	}
	registry.destroy(drag_entity);
}

void UISystem::updateDragItem(entt::registry& registry, float mouse_pos_x, float mouse_pos_y) {
	auto& entity = *registry.view<Drag>().begin();
	auto& motion = registry.get<Motion>(entity);
	motion.position = { mouse_pos_x, mouse_pos_y };
}

bool UISystem::useItemFromInventory(entt::registry& registry, float mouse_pos_x, float mouse_pos_y, Click click) {
	auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
	if (mouse_pos_y >= 50.f - 45.f / 2.f && mouse_pos_x >= 50.f - 45.f / 2.f) {
		int i = (int)((mouse_pos_x - (50.f - 45.f / 2.f)) / 45.f);
		int j = (int)((mouse_pos_y - (50.f - 45.f / 2.f)) / 45.f);
		if (i < 5 && ((registry.view<HiddenInventory>().empty() && j < 4) || j == 0)) {
			auto& inventory_entity = inventory.slots[j * 5 + i];
			auto& inventory_slot = registry.get<InventorySlot>(inventory_entity);
			if (inventory_slot.hasItem) {
				auto& inventory_item = registry.get<Item>(inventory_slot.item);
				// left mouse click
				if (click == Click::LEFT) {
					// use item if no item is being dragged
					if (registry.view<Drag>().empty()) {
						if (j == 0) {
							useItem(registry, inventory_entity, mouse_pos_x, mouse_pos_y);
						}
					}
					else {
						auto& drag_entity = *registry.view<Drag>().begin();
						auto& drag_item = registry.get<Item>(drag_entity);
						// add dragged item to inventory slot if of the same type
						if (registry.get<Item>(inventory_slot.item).type == drag_item.type) {
							if (drag_item.no + inventory_item.no > inventory_slot.capacity) {
								drag_item.no -= inventory_slot.capacity - inventory_item.no;
								inventory_item.no = inventory_slot.capacity;
							}
							else {
								inventory_item.no += drag_item.no;
								registry.destroy(drag_entity);
							}
						}
						// replace the drag item
						else {
							auto previous_item_entity = inventory_slot.item;
							inventory_slot.hasItem = false;
							removeActiveSlot(registry, inventory_entity);
							auto& previous_item = registry.get<Item>(previous_item_entity);
							;							auto item = renderItemAtPos(registry, drag_entity, 50.f + 45.f * i, 50.f + 45.f * j, true, false);
							inventory_slot.hasItem = true;
							inventory_slot.item = item;
							auto& inventory_item = registry.get<Item>(inventory_slot.item);
							if (drag_item.no > inventory_slot.capacity) {
								inventory_item.no = inventory_slot.capacity;
								drag_item.no -= inventory_slot.capacity;
								resetDragItem(registry);
							}
							else {
								inventory_item.no = drag_item.no;
								registry.destroy(drag_entity);
							}
							auto item_entity_on_mouse = renderItemAtPos(registry, previous_item_entity, mouse_pos_x, mouse_pos_y, true, false);
							auto& drag = registry.emplace<Drag>(item_entity_on_mouse);
							drag.noSlot = true;
							auto& item_on_mouse = registry.get<Item>(item_entity_on_mouse);
							item_on_mouse.no = previous_item.no;
							registry.destroy(previous_item_entity);
						}
					}
				}
				// right mouse click
				else {
					// create item at mouse position if no item is being dragged
					if (registry.view<Drag>().empty()) {
						auto item_entity_on_mouse = renderItemAtPos(registry, inventory_slot.item, mouse_pos_x, mouse_pos_y, true, false);
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
						if (item_on_mouse.type == registry.get<Item>(inventory_slot.item).type) {
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
							auto item_entity_on_mouse = renderItemAtPos(registry, inventory_slot.item, mouse_pos_x, mouse_pos_y, true, false);
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
						removeActiveSlot(registry, inventory_entity);
						debug_printf(DebugType::PHYSICS, "Destroying entity (ui sys)\n");
						registry.destroy(inventory_slot.item);
					}
					else if (click == Click::SHIFTRIGHT) {
						if (inventory_item.no == 1) {
							inventory_item.no = 0;
							inventory_slot.hasItem = false;
							removeActiveSlot(registry, inventory_entity);
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
							removeActiveSlot(registry, inventory_entity);
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
							removeActiveSlot(registry, inventory_entity);
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
				if (!registry.view<Drag>().empty() && click == Click::LEFT) {
					auto& drag_entity = *registry.view<Drag>().begin();
					auto& drag_item = registry.get<Item>(drag_entity);
					auto item = renderItemAtPos(registry, drag_entity, 50.f + 45.f * i, 50.f + 45.f * j, true, false);
					inventory_slot.hasItem = true;
					inventory_slot.item = item;
					auto& inventory_item = registry.get<Item>(inventory_slot.item);
					if (drag_item.no > inventory_slot.capacity) {
						inventory_item.no = inventory_slot.capacity;
						drag_item.no -= inventory_slot.capacity;
					}
					else {
						inventory_item.no = drag_item.no;
						registry.destroy(drag_entity);
					}
				}
			}
			return true;
		}
	}
	return false;
}

void UISystem::removeActiveSlot(entt::registry& registry, entt::entity& inventory_slot_entity) {
	if (registry.any_of<ActiveSlot>(inventory_slot_entity)) {
		registry.remove<ActiveSlot>(inventory_slot_entity);
		auto& render_request = registry.get<RenderRequest>(inventory_slot_entity);
		render_request.used_texture = TEXTURE_ASSET_ID::INVENTORY_SLOT;
	}
}

void UISystem::addToInventory(entt::registry& registry, entt::entity& item_entity, FlagSystem& flag_system) {
	auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
	auto& item = registry.get<Item>(item_entity);
	if (registry.all_of<DeathItems>(item_entity)) {
		registry.remove<DeathItems>(item_entity);
	}
	if (item.type != Item::Type::GRAVE) {
		bool pickup = false;
		// check for existing slots having same item type
		for (int i = 0; i < inventory.slots.size(); i++) {
			auto& inventory_slot = registry.get<InventorySlot>(inventory.slots[i]);
			if (inventory_slot.hasItem) {
				auto& inventory_item = registry.get<Item>(inventory_slot.item);
				if (inventory_item.no > inventory_slot.capacity) continue;
				if (inventory_item.type == item.type) {
					int before = inventory_item.no;
					inventory_item.no = std::min(inventory_slot.capacity, inventory_item.no + item.no);
					item.no -= inventory_item.no - before;
					pickup = true;
					if (item.no == 0) {
						MusicSystem::playSoundEffect(SFX::PICKUP);
						flag_system.setMobKilled(true);
						registry.destroy(item_entity);
						return;
					}
				}
			}
		}
		// add item to empty slot
		for (int i = 0; i < inventory.slots.size(); i++) {
			auto& inventory_slot = registry.get<InventorySlot>(inventory.slots[i]);
			if (!inventory_slot.hasItem) {
				inventory_slot.hasItem = true;
				auto& motion = registry.get<Motion>(item_entity);
				if (item.no <= inventory_slot.capacity) {
					inventory_slot.item = item_entity;
					auto& inventory_item = registry.get<Item>(inventory_slot.item);
					inventory_item.no = item.no;
					registry.emplace<UI>(item_entity);
					registry.emplace<FixedUI>(item_entity);
					if (i / 5 > 0 && !registry.view<HiddenInventory>().empty()) {
						registry.emplace<HiddenInventory>(item_entity);
					}
					motion.position = { 50.f + 45.f * (i % 5), 50.f + 45.f * (i / 5) };
					pickup = true;
					break;
				}
				else {
					item.no -= inventory_slot.capacity;
					auto item_entity_2 = renderItemAtPos(registry, item_entity, 50.f + 45.f * (i % 5), 50.f + 45.f * (i / 5), true, false);
					inventory_slot.item = item_entity_2;
					auto& item = registry.get<Item>(item_entity_2);
					item.no = inventory_slot.capacity;
					pickup = true;
				}
			}
		}
		if (pickup) {
			MusicSystem::playSoundEffect(SFX::PICKUP);
			flag_system.setMobKilled(true);
		}
	}
	else {
		registry.destroy(item_entity);
	}
}

void UISystem::clearInventoryAndDrop(entt::registry& registry, float x, float y) {
	auto entity = registry.create();
	auto& item = registry.emplace<Item>(entity);
	item.type = Item::Type::GRAVE;
	registry.emplace<Grave>(entity);
	auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
	for (int i = 0; i < inventory.slots.size(); i++) {
		auto& inventory_slot = registry.get<InventorySlot>(inventory.slots[i]);
		if (inventory_slot.hasItem) {
			auto& inventory_slot_item = registry.get<Item>(inventory_slot.item);
			if (inventory_slot_item.type == Item::Type::DEFAULT_WEAPON ||
				inventory_slot_item.type == Item::Type::HOMING_MISSILE ||
				inventory_slot_item.type == Item::Type::SHOTGUN) {
				continue;
			}
			registry.remove<UI>(inventory_slot.item);
			registry.remove<FixedUI>(inventory_slot.item);
			if (inventory_slot.id > 4 && registry.any_of<HiddenInventory>(inventory_slot.item)) {
				registry.remove<HiddenInventory>(inventory_slot.item);
			}
			auto& motion = registry.get<Motion>(inventory_slot.item);
			motion.position = { x, y };
			registry.emplace<DeathItems>(inventory_slot.item);
			inventory_slot.hasItem = false;
			removeActiveSlot(registry, inventory.slots[i]);
		}
	}
	auto& motion = registry.emplace<Motion>(entity);
	motion.position = { x, y };
	motion.scale = vec2({ 512.f, 512.f }) / 10.f;
	auto& sprite = registry.emplace<Sprite>(entity);
	sprite.dims = { 512.f, 512.f };
	sprite.sheet_dims = { 512.f, 512.f };
	auto& render_request = registry.emplace<RenderRequest>(entity);
	render_request.used_texture = TEXTURE_ASSET_ID::GRAVE;
	render_request.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render_request.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
}

void UISystem::equipItem(entt::registry& registry, Motion& player_motion, FlagSystem& flag_system) {
	if (equip_delay > 2.f) {
		for (auto entity : registry.view<Motion, Item>()) {
			auto& motion = registry.get<Motion>(entity);
			if (abs(player_motion.position.x - motion.position.x) <= 20 && abs(player_motion.position.y - motion.position.y) <= 20) {
				addToInventory(registry, entity, flag_system);
			}
		}
	}
}

void UISystem::creatureDropForMob(entt::registry& registry, entt::entity& entity, DropInfo dropInfo) {
	if (registry.any_of<Drop>(entity)) {
		registry.remove<Drop>(entity);
	}
	auto& drop = registry.emplace<Drop>(entity);
	drop.items.clear();
	for (auto dropItem: dropInfo.dropItems) {
		// roll for item drop
		float randomNo = uniform_dist(rng);
		if (randomNo <= dropItem.probability) {
			Item item;
			item.type = dropItem.type;
			
			auto distribution = std::uniform_int_distribution<int>(dropItem.quantityRange.min, dropItem.quantityRange.max);
			int randomQuantity = distribution(rng);
			item.no = randomQuantity;
			drop.items.push_back(item);
		}
	}
}
