// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "tinyECS/components.hpp"
#include "ui_system.hpp"
#include "music_system.hpp"
#include "util/debug.hpp"
#include "map/map_system.hpp"
// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

// create the world
WorldSystem::WorldSystem(entt::registry& reg, PhysicsSystem& physics_system, FlagSystem& flag_system, QuadTree& quadTree) :
	registry(reg),
	physics_system(physics_system),
	flag_system(flag_system), 
	quadTree(quadTree)
{
	for (auto i = 0; i < KeyboardState::NUM_STATES; i++) key_state[i] = false;
	player_entity = createPlayer(registry, {0, 0});
	ship_entity = createShip(registry, {0, 0});
	main_camera_entity = createCamera(registry, player_entity);

	screen_entity = registry.create();
	registry.emplace<ScreenState>(screen_entity);
	auto& screen_state = registry.get<ScreenState>(screen_entity);
	screen_state.current_screen = ScreenState::ScreenType::TITLE;
	createTitleScreen(registry);
	createPlayerHealthBar(registry);
	createInventory(registry);

	// seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	MusicSystem::clear();

	// Destroy all created components
	registry.clear();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		std::cerr << error << ": " << desc << std::endl;
	}
}

// call to close the window, wrapper around GLFW commands
void WorldSystem::close_window() {
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {

	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		std::cerr << "ERROR: Failed to initialize GLFW in world_system.cpp" << std::endl;
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// CK: setting GLFW_SCALE_TO_MONITOR to true will rescale window but then you must handle different scalings
	// glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);		// GLFW 3.3+
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_FALSE);		// GLFW 3.3+

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX, "Nova", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "ERROR: Failed to glfwCreateWindow in world_system.cpp" << std::endl;
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_button_pressed_redirect = [](GLFWwindow* wnd, int _button, int _action, int _mods) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button_pressed(_button, _action, _mods); };
	
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_pressed_redirect);

	return window;
}

void WorldSystem::init() {
	// start playing background music indefinitely
	debug_printf(DebugType::GAME_INIT, "Starting music...\n");
	MusicSystem::playMusic(Music::FOREST);
	// Set all states to default

	// TODO: can move this next block into restart_game, but needs some debugging...
	//       it put the player perma-stuck on the intiial tutorial screen on death

	int w, h;
    glfwGetFramebufferSize(window, &w, &h);

	// init everything for the main upgrade screen
	createButton(registry, vec2(w/6 - w/2*0.15f, h/4), vec2(w/6 - w/2*0.21f, w/6 - w/2*0.21f), ButtonOption::Option::SHIP, "Ship"); 
	createButton(registry, vec2(w/4, h/4), vec2(w/6 - w/2*0.21f, w/6 - w/2*0.21f), ButtonOption::Option::PLAYER, "Player"); 
	createButton(registry, vec2(2*w/6 + w/2*0.15f, h/4), vec2(w/6 - w/2*0.21f, w/6 - w/2*0.21f), ButtonOption::Option::WEAPON, "Weapons"); 

	createIcon(registry, vec2(2*w/6 - w/2*0.48f, h/4 - h/2*0.005f), vec2(w/6 - w/2*0.23f, w/6 - w/2*0.23f), 1, vec2(128.0f, 128.0f), vec2(128.0f, 128.0f)); 
	createIcon(registry, vec2(w/4, h/4), PLAYER_SPRITESHEET.dims * vec2(1.8f, 1.8f), 0, PLAYER_SPRITESHEET.dims, PLAYER_SPRITESHEET.sheet_dims); 


	// init all the ui ship stuff
	createUIShip(registry, vec2(w/4, h/4), vec2(w/6 - w/2*0.12f, w/6 - w/2*0.12f), 4);
	// smg weapon
	createUIShipWeapon( registry, 
						vec2(w/4, h/4), 
						vec2(w/6 - w/2*0.15f, w/6 - w/2*0.12f),
						vec2(48.f, 48.f), 
						vec2(336.f, 48.f), 
						{0, 0}, 
						8 );
	// smg engine
	createUIShipEngine(registry, vec2(w/4, h/4 + h/2*0.01f), vec2(w/6 - w/2*0.15f, w/6 - w/2*0.15f), 12);

	// health
	createUpgradeButton(registry, vec2(w/4 - w/2*0.27f, h/4 - h/2*0.11f), vec2(w/2*0.05f, h/2*0.025f), ButtonOption::Option::SHIP_HEALTH_UPGRADE, TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE);
	// blaster
	createUpgradeButton(registry, vec2(w/4 + w/2*0.3f, h/4 - h/2*0.07f), vec2(w/2*0.05f, h/2*0.025f), ButtonOption::Option::SHIP_BLASTER_UPGRADE, TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE);
	// range
	createUpgradeButton(registry, vec2(w/4 - w/2*0.26f, h/4 + h/2*0.3f), vec2(w/2*0.05f, h/2*0.025f), ButtonOption::Option::SHIP_RANGE_UPGRADE, TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE);
	// fire rate
	createUpgradeButton(registry, vec2(w/4 + w/2*0.33f, h/4 + h/2*0.25f), vec2(w/2*0.05f, h/2*0.025f), ButtonOption::Option::SHIP_FIRERATE_UPGRADE, TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE);

	// init all of the text boxes for the tutorial
	textBoxEntities.resize(5);
    vec2 size = {0.4f, 3.0f};
    textBoxEntities[0] = createTextBox(registry, vec2(1.0f, 200.0f), size, 
        "Welcome to Nova! Use the 'W', 'A', 'S', 'D' keys to move around!", 0.35f, {1.0f, 1.0f, 1.0f});
    textBoxEntities[1] = createTextBox(registry, vec2(1.0f, 200.0f), size, 
        "Great! Press 'F' near the ship to access or leave the ship upgrade", 0.35f, {1.0f, 1.0f, 1.0f});
    textBoxEntities[2] = createTextBox(registry, vec2(1.0f, 200.0f), size, 
        "Good job! Now use left click to firing your weapon.", 0.35f, {1.0f, 1.0f, 1.0f});
    textBoxEntities[3] = createTextBox(registry, vec2(1.0f, 200.0f), size, 
        "Nice shot! Go explore the planet.", 0.35f, {1.0f, 1.0f, 1.0f});
    textBoxEntities[4] = createTextBox(registry, vec2(1.0f, 200.0f), size, 
        "You defeated an enemy! Keep exploring.", 0.35f, {1.0f, 1.0f, 1.0f});
    
    // make them all inactive initially
    // for (auto entity : textBoxEntities) {
    //     auto& textData = registry.get<TextData>(entity);
    //     textData.active = false;
    // }
    
    // // then set only the first one to active
    // auto& firstTextData = registry.get<TextData>(textBoxEntities[0]);
    // firstTextData.active = true;
	
    //---------------------------------------
	// reset all the text boxes
    for (auto entity : textBoxEntities) {
        auto& textData = registry.get<TextData>(entity);
        textData.active = false;
    }
    auto& firstTextData = registry.get<TextData>(textBoxEntities[0]);
    firstTextData.active = true;
	flag_system.reset();

	// reset the timer for the last box
	mobKilledTextTimer = 0.0;
	//---------------------------------------

    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	click_delay += elapsed_ms_since_last_update / 1000.f;
	UISystem::equip_delay += elapsed_ms_since_last_update / 1000.f;
	auto screen_state = registry.get<ScreenState>(screen_entity);
	if (screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI) {
		return true;
	}
	if (screen_state.current_screen == ScreenState::ScreenType::TITLE) {
		click_delay = 0;
		return true; 
	}
	

	auto player = registry.get<Player>(player_entity);
	if (player.health <= 0) {
		debug_printf(DebugType::WORLD, "Game over; restarting game now...\n");
		if (!registry.view<Grave>().empty()) {
			registry.destroy(*registry.view<Grave>().begin());
		}
		if (!registry.view<DeathItems>().empty()) {
			for (auto entity : registry.view<DeathItems>()) {
				registry.destroy(entity);
			}
		}
		auto& motion = registry.get<Motion>(player_entity);
		UISystem::clearInventoryAndDrop(registry, motion.position.x, motion.position.y);
		restart_game();
	}

	InputState i; 
	if (key_state[KeyboardState::UP]) i.up = true;
	if (key_state[KeyboardState::DOWN]) i.down = true;
	if (key_state[KeyboardState::LEFT]) i.left = true;
	if (key_state[KeyboardState::RIGHT]) i.right = true;
	physics_system.updatePlayerVelocity(i);

	// TODO: move left-mouse-click polling logic
	mouse_click_poll -= elapsed_ms_since_last_update;
	if (mouse_click_poll < 0) {
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (state == GLFW_PRESS) left_mouse_click();
		mouse_click_poll = MOUSE_POLL_RATE;
	}


	// TODO: move direction system
	auto dir_view = registry.view<Motion, Sprite>();
	for (auto& entity : dir_view) {
		auto& motion = registry.get<Motion>(entity);
		auto& sprite = registry.get<Sprite>(entity);

		if (length(motion.velocity) > 0.0f) {
			vec2 velo = motion.velocity;
			float x_scale = abs(motion.scale.x);

			if (abs(velo.y) > 0) {
				sprite.coord.row = (velo.y > 0) ? sprite.down_row : sprite.up_row;
				motion.scale.x = x_scale;
			}

			if (abs(velo.x) > 0) {
				sprite.coord.row = sprite.right_row;
				motion.scale.x = (velo.x < 0) ? -1.f * x_scale : x_scale;
			}
		}
	}

	auto& p_motion = registry.get<Motion>(player_entity);
	auto& p_sprite = registry.get<Sprite>(player_entity);

	if (length(p_motion.velocity) > 0.0f) {
		vec2 velo = p_motion.velocity;
		float x_scale = abs(p_motion.scale.x);

		if (abs(velo.x) > 0) {
			p_sprite.coord.row = 1;
			p_motion.scale.x = (velo.x < 0) ? -1.f * x_scale : x_scale;
		}

		if (abs(velo.y) > 0) {
			p_sprite.coord.row = (velo.y > 0) ? 0 : 2;
			p_motion.scale.x = x_scale;
		}
	}

	// TODO: check if ENEMY is within the range of the ship, and have it shoot towards that direction
	auto &ship = registry.get<Ship>(ship_entity);
	auto mobs = registry.view<Mob>();

	float elapsed_s = elapsed_ms_since_last_update / 1000;
	ship.timer -= elapsed_s;

	if (ship.timer <= 0) {
		ship.timer = SHIP_TIMER_S;
		
		for (auto entity : mobs) {
			auto motion = registry.get<Motion>(entity);
			auto shipMotion = registry.get<Motion>(ship_entity);

			glm::vec2 shipPos = glm::vec2(shipMotion.position.x, shipMotion.position.y);
			glm::vec2 enemyPos = glm::vec2(motion.position.x, motion.position.y);

			if (glm::distance(shipPos, enemyPos) <= ship.range) {
				vec2 direction = normalize(enemyPos - shipPos);
				vec2 velocity = direction * PROJECTILE_SPEED;
				vec2 smg_velocity = direction * SMG_PROJ_SPEED;
				vec2 missle_velocity = direction * MISSLE_PROJ_SPEED;
				vec2 blaster_velocity = direction * BLASTER_PROJ_SPEED;
				vec2 railgun_velocity = direction * RAILGUN_PROJ_SPEED;
				switch (bulletType) {
					case BulletType::GOLD_PROJECTILE:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity, PROJECTILE_DAMAGE, TEXTURE_ASSET_ID::GOLD_PROJECTILE);
						break;
					case BulletType::BLASTER_PROJECTILE:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), blaster_velocity, BLASTER_PROJ_DAMAGE, TEXTURE_ASSET_ID::BLASTER_PROJECTILE);
						break;
					case BulletType::MISSLE_PROJECTILE:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE/1.5, PROJECTILE_SIZE*2), missle_velocity, MISSLE_PROJ_DAMAGE, TEXTURE_ASSET_ID::MISSLE_PROJECTILE);
						break;
					case BulletType::RAILGUN_PROJECTILE:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE/1.5, PROJECTILE_SIZE*2), railgun_velocity, RAILGUN_PROJ_DAMAGE, TEXTURE_ASSET_ID::RAILGUN_PROJECTILE);
						break;
					case BulletType::SMG_PROJECTILE:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE/1.5, PROJECTILE_SIZE*2), smg_velocity, SMG_PROJ_DAMAGE, TEXTURE_ASSET_ID::SMG_PROJECTILE);
						break;
					default:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity, PROJECTILE_DAMAGE, TEXTURE_ASSET_ID::GOLD_PROJECTILE);
						break;
				}
			}
		}
	}

	// TODO: freeze everything if in ship_ui
	
	MapSystem::update_location(registry, player_entity);
  
	for (auto entity : registry.view<Projectile>()) {
		auto& projectile = registry.get<Projectile>(entity);
		projectile.timer -= elapsed_ms_since_last_update;
		if (projectile.timer <= 0) {
			debug_printf(DebugType::PHYSICS, "Destroying entity (world sys: projectile)\n");

			registry.destroy(entity);
		}
	}

	// TODO: move attack cooldown system
	auto& player_comp = registry.get<Player>(player_entity);
	player_comp.weapon_cooldown = max(0.f, player_comp.weapon_cooldown - elapsed_s);

	// TODO: move enemy attack cooldown system
	for (auto&& [entity, mob] : registry.view<Mob>().each()) {
		mob.hit_time -= elapsed_s;
	}	

	// handle the text boxes for tutorial
	handleTextBoxes(elapsed_ms_since_last_update);

	return true;
}

void WorldSystem::player_respawn() {
	Player& player = registry.get<Player>(player_entity);
	player.health = PLAYER_HEALTH;

	Motion& player_motion = registry.get<Motion>(player_entity);
	player_motion.velocity = {0.f, 0.f};
	player_motion.acceleration = {0.f, 0.f};
	UISystem::updatePlayerHealthBar(registry, PLAYER_HEALTH);
}

void WorldSystem::handleTextBoxes(float elapsed_ms_since_last_update) {
	FlagSystem::TutorialStep currentStep = flag_system.getTutorialStep();
    
	// gets rid of the last text box after 5 seconds
	if (currentStep == FlagSystem::TutorialStep::Shot) {
		mobKilledTextTimer += elapsed_ms_since_last_update / 1000.0f;
		if (mobKilledTextTimer > 5.0f) {
			for (auto entity : textBoxEntities) {
				auto& textData = registry.get<TextData>(entity);
				textData.active = false;
			}
			return;
		}
	}

    // make all text boxes inactive
    for (auto entity : textBoxEntities) {
        auto& textData = registry.get<TextData>(entity);
        textData.active = false;
    }
    
    // activate only the appropriate one
    int activeIndex = -1;
    switch (currentStep) {
        case FlagSystem::TutorialStep::None:
            activeIndex = 0;
            break;
        case FlagSystem::TutorialStep::Moved:
            activeIndex = 1;
            break;
        case FlagSystem::TutorialStep::Accessed:
            activeIndex = 2;
            break;
        case FlagSystem::TutorialStep::Shot:
            activeIndex = 3;
            break;
        case FlagSystem::TutorialStep::MobKilled:
            activeIndex = 4;
            break;
    }
    
	// activate specific text box to true
    if (activeIndex >= 0 && activeIndex < textBoxEntities.size()) {
        auto& textData = registry.get<TextData>(textBoxEntities[activeIndex]);
        textData.active = true;
    }
}


// Reset the world state to its initial state
void WorldSystem::restart_game() {
	debug_printf(DebugType::WORLD, "Restarting...\n");

	auto& screen_state = registry.get<ScreenState>(registry.view<ScreenState>().front());
	screen_state.darken_screen_factor = 0;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	// auto motions = registry.view<Motion>(entt::exclude<Player, Ship, UIShip, Background, Title, TextData>);	
	auto motions = registry.view<Motion>(entt::exclude<Player, Ship, Background, DeathItems, Grave, ShipWeapon, ShipEngine>);
	for (auto entity : motions) {
		if (registry.any_of<FixedUI>(entity)) {
			if (registry.any_of<Item>(entity)) {
				registry.destroy(entity);
			}
			continue;
		}
		else registry.destroy(entity);
	}
	// auto motions = registry.view<Motion>(entt::exclude<Player, Ship, Background, FixedUI, DeathItems, Grave, ShipWeapon>);
	// registry.destroy(motions.begin(), motions.end());
	vec2& p_pos = registry.get<Motion>(player_entity).position;
	vec2& s_pos = registry.get<Motion>(ship_entity).position;

	// reset ui ship to default ---> not sure if we wanna do this?
	// for (auto ui_ship_entity : registry.view<UIShip>()) {
	// 	auto& ui_ship_render = registry.get<RenderRequest>(ui_ship_entity);
	// 	ui_ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_VERY_DAMAGE;
	// }
	// auto& ship_render = registry.get<RenderRequest>(ship_entity);
	// ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_VERY_DAMAGE;

	MapSystem::populate_ecs(registry, p_pos, s_pos);
	
	player_respawn();
	/*createPlayerHealthBar(registry, p_pos);
	createInventory(registry);*/
	quadTree.initTree(registry); 
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// on key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	auto& screen_state = registry.get<ScreenState>(screen_entity);

	// title screen
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		//close_window();
		if (screen_state.current_screen == ScreenState::ScreenType::TITLE) {
			debug_printf(DebugType::USER_INPUT, "Closing pause title screen\n");
			screen_state.current_screen = ScreenState::ScreenType::GAMEPLAY;
		}
		else {
			debug_printf(DebugType::USER_INPUT, "Opening pause title screen\n");
			screen_state.current_screen = ScreenState::ScreenType::TITLE;
		}
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
	}

	// TODO: refactor player movement logic. Also, could allow for rebinding keyboard mapping in
	//       a settings menu
	if (key == GLFW_KEY_UP    || key == GLFW_KEY_W) key_state[KeyboardState::UP]    = (action != GLFW_RELEASE);
	if (key == GLFW_KEY_DOWN  || key == GLFW_KEY_S) key_state[KeyboardState::DOWN]  = (action != GLFW_RELEASE);
	if (key == GLFW_KEY_LEFT  || key == GLFW_KEY_A) key_state[KeyboardState::LEFT]  = (action != GLFW_RELEASE);
	if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) key_state[KeyboardState::RIGHT] = (action != GLFW_RELEASE);

	if (key == GLFW_KEY_P) {
		auto debugView = registry.view<Debug>();
		if (debugView.empty()) {
			registry.emplace<Debug>(player_entity);
		}
		else {
			for (auto entity : debugView) {
				std::cout << "Removing debug" << std::endl;
				registry.remove<Debug>(entity);
			}
		}
	}

	

	// F to toggle opening/closing ship ui
	if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
        if (screen_state.current_screen == ScreenState::ScreenType::GAMEPLAY) {
            auto& player_motion = registry.get<Motion>(player_entity);
            auto& ship_motion = registry.get<Motion>(ship_entity);

			player_motion.velocity.x = 0;
			player_motion.velocity.y = 0;

            float distance_to_ship = glm::distance(player_motion.position, ship_motion.position);
            if (distance_to_ship < 150.0f) {
				debug_printf(DebugType::USER_INPUT, "Opening Upgrade UI\n");
                screen_state.current_screen = ScreenState::ScreenType::UPGRADE_UI;
            }
        } else if (screen_state.current_screen == ScreenState::ScreenType::UPGRADE_UI) {
			debug_printf(DebugType::USER_INPUT, "Closing Upgrade UI\n");
            screen_state.current_screen = ScreenState::ScreenType::GAMEPLAY;
        } else if (screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI) {
			debug_printf(DebugType::USER_INPUT, "Closing Ship Upgrade UI\n");
            // screen_state.current_screen = ScreenState::ScreenType::GAMEPLAY;
			screen_state.current_screen = ScreenState::ScreenType::UPGRADE_UI;
        }
    }

	if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
		auto& inventory = registry.get<Inventory>(*registry.view<Inventory>().begin());
		if (registry.view<HiddenInventory>().empty()) {
			for (int i = 5; i < inventory.slots.size(); i++) {
				registry.emplace<HiddenInventory>(inventory.slots[i]);
				auto& inventory_slot = registry.get<InventorySlot>(inventory.slots[i]);
				if (inventory_slot.hasItem) {
					registry.emplace<HiddenInventory>(inventory_slot.item);
				}
			}
		}
		else {
			for (int i = 5; i < inventory.slots.size(); i++) {
				registry.remove<HiddenInventory>(inventory.slots[i]);
				auto& inventory_slot = registry.get<InventorySlot>(inventory.slots[i]);
				if (inventory_slot.hasItem) {
					registry.remove<HiddenInventory>(inventory_slot.item);
				}
			}
		}
	}

	//// TODO: testing sound system. remove this later
	//if (key == GLFW_KEY_1) MusicSystem::playMusic(Music::FOREST, -1, 200);
	//if (key == GLFW_KEY_2) MusicSystem::playMusic(Music::BEACH, -1, 200);
	//if (key == GLFW_KEY_3) MusicSystem::playMusic(Music::SNOWLANDS, -1, 200);
	//if (key == GLFW_KEY_4) MusicSystem::playMusic(Music::SAVANNA, -1, 200);
	//if (key == GLFW_KEY_5) MusicSystem::playMusic(Music::OCEAN, -1, 200);
	//if (key == GLFW_KEY_6) MusicSystem::playMusic(Music::JUNGLE, -1, 200);

	if (key == GLFW_KEY_1 && action == GLFW_RELEASE) UISystem::useItemFromInventory(registry, 50.f, 50.f, Click::LEFT);
	if (key == GLFW_KEY_2 && action == GLFW_RELEASE) UISystem::useItemFromInventory(registry, 95.f, 50.f, Click::LEFT);
	if (key == GLFW_KEY_3 && action == GLFW_RELEASE) UISystem::useItemFromInventory(registry, 140.f, 50.f, Click::LEFT);
	if (key == GLFW_KEY_4 && action == GLFW_RELEASE) UISystem::useItemFromInventory(registry, 185.f, 50.f, Click::LEFT);
	if (key == GLFW_KEY_5 && action == GLFW_RELEASE) UISystem::useItemFromInventory(registry, 230.f, 50.f, Click::LEFT);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {

	// record the current mouse position
	mouse_pos_x = mouse_position.x;
	mouse_pos_y = mouse_position.y;
	auto& screen_state = registry.get<ScreenState>(screen_entity);
	if (screen_state.current_screen == ScreenState::ScreenType::TITLE) {
		for (auto entity : registry.view<ButtonOption>(entt::exclude<Button>)) {
			auto& title_option = registry.get<ButtonOption>(entity);
			title_option.hover = abs(mouse_pos_x - title_option.position.x) <= title_option.size.x / 2 &&
				abs(mouse_pos_y - title_option.position.y) <= title_option.size.y / 2;
		}
	} else if (screen_state.current_screen == ScreenState::ScreenType::UPGRADE_UI) {
		for (auto entity : registry.view<Button>(entt::exclude<Title>)) {
			auto& upgrade_ui_option = registry.get<ButtonOption>(entity);
			upgrade_ui_option.hover = abs(mouse_pos_x - upgrade_ui_option.position.x) <= upgrade_ui_option.size.x / 2 &&
				abs(mouse_pos_y - upgrade_ui_option.position.y) <= upgrade_ui_option.size.y / 2;
		}
	} else if (screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI) {
		for (auto entity : registry.view<UpgradeButton>(entt::exclude<Title, Button>)) {
			auto& upgrade_option = registry.get<ButtonOption>(entity);
			upgrade_option.hover = abs(mouse_pos_x - upgrade_option.position.x) <= upgrade_option.size.x / 2 &&
				abs(mouse_pos_y - upgrade_option.position.y) <= upgrade_option.size.y / 2;
		}
	}
	if (!registry.view<Drag>().empty()) {
		UISystem::updateDragItem(registry, mouse_pos_x, mouse_pos_y);
	}
}

void WorldSystem::right_mouse_click(int mods) {
	bool itemUsed = false;

	if (click_delay > 0.3f) {
		if (mods & GLFW_MOD_CONTROL) {
			itemUsed = UISystem::useItemFromInventory(registry, mouse_pos_x, mouse_pos_y, Click::CTRLRIGHT);
		}
		else if (mods & GLFW_MOD_SHIFT) {
			itemUsed = UISystem::useItemFromInventory(registry, mouse_pos_x, mouse_pos_y, Click::SHIFTRIGHT);
		}
		else if (mods & GLFW_MOD_ALT) {
			itemUsed = UISystem::useItemFromInventory(registry, mouse_pos_x, mouse_pos_y, Click::ALTRIGHT);
		}
		else {
			itemUsed = UISystem::useItemFromInventory(registry, mouse_pos_x, mouse_pos_y, Click::RIGHT);
		}
		if (itemUsed) {
			click_delay = 0.0f;
		}
	}

	if (!registry.view<Drag>().empty() && click_delay > 0.3f && !itemUsed) {
		UISystem::resetDragItem(registry);
		click_delay = 0.0f;
	}
}

void WorldSystem::left_mouse_click() {
	auto& player_motion = registry.get<Motion>(player_entity);
	vec2 player_to_mouse_direction = vec2(mouse_pos_x, mouse_pos_y) - vec2(WINDOW_WIDTH_PX / 2, WINDOW_HEIGHT_PX / 2);
	vec2 direction = normalize(player_to_mouse_direction); // player position is always at (0, 0) in camera space
	vec2 velocity = direction * PROJECTILE_SPEED;

	auto& player_comp = registry.get<Player>(player_entity);
	auto& screen_state = registry.get<ScreenState>(screen_entity);

	int w, h;
    glfwGetFramebufferSize(window, &w, &h);

	if (screen_state.current_screen == ScreenState::ScreenType::TITLE) {
		for (auto entity : registry.view<ButtonOption>(entt::exclude<Button>)) {
			auto& title_option = registry.get<ButtonOption>(entity);
			if (title_option.hover) {
				MusicSystem::playSoundEffect(SFX::SELECT);
				if (title_option.type == ButtonOption::Option::PLAY) {
					screen_state.current_screen = ScreenState::ScreenType::GAMEPLAY;
					return;
				}
				else if (title_option.type == ButtonOption::Option::EXIT) {
					close_window();
				}
				else if (title_option.type == ButtonOption::Option::RESTART) {
					screen_state.current_screen = ScreenState::ScreenType::GAMEPLAY;
					restart_game();
					return;
				}
				title_option.hover = false;
			}
			
		}
	} else if (screen_state.current_screen == ScreenState::ScreenType::UPGRADE_UI) {
		for (auto entity : registry.view<Button>()) {
			auto& title_option = registry.get<ButtonOption>(entity);
			if (title_option.hover) {
				// TODO: handle all the other upgrade screens
				MusicSystem::playSoundEffect(SFX::SELECT);
				if (title_option.type == ButtonOption::Option::SHIP) {
					screen_state.current_screen = ScreenState::ScreenType::SHIP_UPGRADE_UI;
					return;
				}
				// else if (title_option.type == ButtonOption::Option::EXIT) {
				// 	close_window();
				// }
				// else if (title_option.type == ButtonOption::Option::RESTART) {
				// 	screen_state.current_screen = ScreenState::ScreenType::GAMEPLAY;
				// 	restart_game();
				// 	return;
				// }
				title_option.hover = false;
			}
			
		}
	} else if (screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI) {
		for (auto entity : registry.view<UpgradeButton>()) {
			auto& upgrade_option = registry.get<ButtonOption>(entity);
			auto& upgrade_render = registry.get<RenderRequest>(entity);
			if (upgrade_option.hover) {
				upgrade_render.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_PRESSED;
				MusicSystem::playSoundEffect(SFX::SELECT);
				if (upgrade_option.type == ButtonOption::Option::SHIP_HEALTH_UPGRADE) {
					for (auto ui_ship_entity : registry.view<UIShip>()) {
						auto& ui_ship_render = registry.get<RenderRequest>(ui_ship_entity);
						if (ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_VERY_DAMAGE) {
							ui_ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_DAMAGE;
						} else if (ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_DAMAGE) {
							ui_ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_SLIGHT_DAMAGE;
						} else if (ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_SLIGHT_DAMAGE) {
							ui_ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_FULL_HP;
						} else {
							ui_ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_FULL_HP;
						}
					}
					for (auto ship_entity : registry.view<Ship>()) {
						auto& ship_render = registry.get<RenderRequest>(ship_entity);
						auto& ship = registry.get<Ship>(ship_entity);
						if (ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_VERY_DAMAGE) {
							ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_DAMAGE;
							ship.health += SHIP_HEALTH_UPGRADE;
						} else if (ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_DAMAGE) {
							ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_SLIGHT_DAMAGE;
							ship.health += SHIP_HEALTH_UPGRADE;
						} else if (ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_SLIGHT_DAMAGE) {
							ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_FULL_HP;
							ship.health += SHIP_HEALTH_UPGRADE;
						} else {
							ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_FULL_HP;
						}
					}
				}
				if (upgrade_option.type == ButtonOption::Option::SHIP_BLASTER_UPGRADE) {
					// smg --> missles --> blaster --> railgun
					// TODO: change the type of bullet that the ship shoots as well
					// TODO: make sure this upgrade in the ship ui is reflected in the actual game as well.
					for (auto ui_ship_weapon_entity : registry.view<UIShipWeapon>()) {
						// TODO: change the type of bullet that the ship shoots as well
						// TODO: make sure this upgrade in the ship ui is reflected in the actual game as well.
						auto& ui_ship_weapon = registry.get<UIShipWeapon>(ui_ship_weapon_entity);
						auto& ui_ship_render = registry.get<RenderRequest>(ui_ship_weapon_entity);
						vec2& ship_pos = registry.get<Motion>(ship_entity).position;
						if (ui_ship_weapon.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_SMG_WEAPON) {
							// change to missles
							registry.destroy(ui_ship_weapon_entity);
							entt::entity curr = createUIShipWeapon( registry, 
																	vec2(w/4 + w/2/84, h/4),
																	vec2(w/6 - w/2/8.4, w/6 - w/2/8.4),
																	vec2(51.f, 48.f), 
																	vec2(816.f, 48.f), 
																	{0, 0}, 
																	6 );
							auto& new_ship_weapon = registry.get<UIShipWeapon>(curr);
							new_ship_weapon.active = true;

							// update gameplay ship
							auto shipWeapon = registry.view<ShipWeapon>();
							registry.destroy(shipWeapon.begin(), shipWeapon.end());
							createShipWeapon( registry, 
								vec2(ship_pos.x + 2.0f, ship_pos.y - 10.0f),
								vec2(100, 115),
								vec2(18.f, 48.f), 
								vec2(336.f, 48.f), 
								{0, 0}, 
								6 );
							
							// change the bullet type
							bulletType = BulletType::MISSLE_PROJECTILE;

						} else if (ui_ship_weapon.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_MISSLES_WEAPON) {
							// change to blaster
							registry.destroy(ui_ship_weapon_entity);
							entt::entity curr = createUIShipWeapon( registry, 
																	vec2(w/4 - w/2/22.7, h/4),
																	vec2(w/6 - w/2/4.7, w/6 - w/2/9.3),
																	vec2(51.f, 48.f), 
																	vec2(816.f, 48.f), 
																	{0, 0}, 
																	5 );
							auto& new_ship_weapon = registry.get<UIShipWeapon>(curr);
							new_ship_weapon.active = true;

							// update gameplay ship
							auto shipWeapon = registry.view<ShipWeapon>();
							registry.destroy(shipWeapon.begin(), shipWeapon.end());
							createShipWeapon( registry, 
								// vec2(1640, 1330), 
								vec2(ship_pos.x + 5.0f, ship_pos.y - 25.0f),
								vec2(60, 120),
								vec2(20.f, 48.f), 
								vec2(336.f, 48.f), 
								{0, 0}, 
								5 );
							
							// change the bullet type
							bulletType = BulletType::BLASTER_PROJECTILE;

						} else if (ui_ship_weapon.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_BLASTER_WEAPON) {
							// change to railgun
							registry.destroy(ui_ship_weapon_entity);
							entt::entity curr = createUIShipWeapon( registry, 
																	vec2(w/4 - w/2/42, h/4),
																	vec2(w/6 - w/2/6, w/6 - w/2/7.6),
																	vec2(51.f, 48.f), 
																	vec2(816.f, 48.f), 
																	{0, 0}, 
																	7 );
							auto& new_ship_weapon = registry.get<UIShipWeapon>(curr);
							new_ship_weapon.active = true;

							// update gameplay ship
							auto shipWeapon = registry.view<ShipWeapon>();
							registry.destroy(shipWeapon.begin(), shipWeapon.end());
							createShipWeapon( registry, 
								vec2(ship_pos.x, ship_pos.y),
								vec2(110, 100),
								vec2(24.f, 48.f), 
								vec2(336.f, 48.f), 
								{0, 0}, 
								7 );
							
							// change the bullet type
							bulletType = BulletType::RAILGUN_PROJECTILE;

						} else if (!ui_ship_weapon.active) {
							// create a new weapon (start as smg)
							registry.destroy(ui_ship_weapon_entity);
							entt::entity curr = createUIShipWeapon( registry, 
																	vec2(w/4, h/4), 
																	vec2(w/6 - w/2/6.7, w/6 - w/2/8.4),
																	vec2(48.f, 48.f), 
																	vec2(336.f, 48.f), 
																	{0, 0}, 
																	8 );
							auto& new_ship_weapon = registry.get<UIShipWeapon>(curr);
							new_ship_weapon.active = true;

							// update gameplay ship
							auto shipWeapon = registry.view<ShipWeapon>();
							registry.destroy(shipWeapon.begin(), shipWeapon.end());
							createShipWeapon( registry, 
								// vec2(1590, 1345), 
								vec2(ship_pos.x, ship_pos.y),
								vec2(120, 110),
								vec2(48.f, 48.f), 
								vec2(336.f, 48.f), 
								{0, 0}, 
								8 );
							
							// change the bullet type
							bulletType = BulletType::SMG_PROJECTILE;
						}
					}
				}
				if (upgrade_option.type == ButtonOption::Option::SHIP_FIRERATE_UPGRADE) {
					// smg engine --> missles engine --> blaster engine --> railgun engine
					for (auto ui_ship_engine_entity : registry.view<UIShipEngine>()) {
						auto& ui_ship_engine = registry.get<UIShipEngine>(ui_ship_engine_entity);
						auto& ui_ship_render = registry.get<RenderRequest>(ui_ship_engine_entity);
						vec2& ship_pos = registry.get<Motion>(ship_entity).position;
						auto& ship = registry.get<Ship>(ship_entity);
						if (ui_ship_engine.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_SMG_ENGINE) {
							// change to missles engine
							registry.destroy(ui_ship_engine_entity);
							entt::entity chosenEngine = createUIShipEngine(registry, 
								vec2(w/4, h/4 + h/2/37.3),
								vec2(w/6 - w/2/6.7, w/6 - w/2/6.7),
								10);
							auto& currEngine = registry.get<UIShipEngine>(chosenEngine);
							currEngine.active = true;

							// update gameplay ship
							auto shipEngine = registry.view<ShipEngine>();
							registry.destroy(shipEngine.begin(), shipEngine.end());
							createShipEngine(registry, vec2(ship_pos.x - 13.0f, ship_pos.y) , vec2(240.0f - 125.0f, 137.5f - 35.0f), 10);

							ship.timer += SHIP_TIMER_UPGRADE;
						} else if (ui_ship_engine.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_MISSLE_ENGINE) {
							// change to blaster engine
							registry.destroy(ui_ship_engine_entity);
							entt::entity chosenEngine = createUIShipEngine(registry, 
								vec2(w/4, h/4 + h/2/37.3),
								vec2(w/6 - w/2/6.7, w/6 - w/2/6.7),
								9);
							auto& currEngine = registry.get<UIShipEngine>(chosenEngine);
							currEngine.active = true;

							// update gameplay ship
							auto shipEngine = registry.view<ShipEngine>();
							registry.destroy(shipEngine.begin(), shipEngine.end());
							createShipEngine(registry, vec2(ship_pos.x - 13.0f, ship_pos.y) , vec2(240.0f - 125.0f, 137.5f - 35.0f), 9);

							ship.timer += SHIP_TIMER_UPGRADE;
						} else if (ui_ship_engine.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_BLASTER_ENGINE) {
							// change to railgun engine
							registry.destroy(ui_ship_engine_entity);
							entt::entity chosenEngine = createUIShipEngine(registry, 
								vec2(w/4, h/4 + h/2/56),
								vec2(w/6 - w/2/6.7, w/6 - w/2/6.7),
								11);
							auto& currEngine = registry.get<UIShipEngine>(chosenEngine);
							currEngine.active = true;

							// update gameplay ship
							auto shipEngine = registry.view<ShipEngine>();
							registry.destroy(shipEngine.begin(), shipEngine.end());
							createShipEngine(registry, vec2(ship_pos.x - 10.0f, ship_pos.y) , vec2(240.0f - 125.0f, 137.5f - 35.0f), 11);

							ship.timer += SHIP_TIMER_UPGRADE;
						} else if (!ui_ship_engine.active) {
							// create a new weapon (start as smg engine)
							registry.destroy(ui_ship_engine_entity);
							entt::entity chosenEngine = createUIShipEngine(registry, 
								vec2(w/4, h/4 + h/2/112),
								vec2(w/6 - w/2/6.7, w/6 - w/2/6.7),
								12);
							auto& currEngine = registry.get<UIShipEngine>(chosenEngine);
							currEngine.active = true;
							
							// update gameplay ship
							auto shipEngine = registry.view<ShipEngine>();
							registry.destroy(shipEngine.begin(), shipEngine.end());
							createShipEngine(registry, vec2(ship_pos.x - 13.0f, ship_pos.y) , vec2(240.0f - 125.0f, 137.5f - 35.0f), 12);

							ship.timer += SHIP_TIMER_UPGRADE;
						}
					}
				}
				if (upgrade_option.type == ButtonOption::Option::SHIP_RANGE_UPGRADE) {
					auto& ship = registry.get<Ship>(ship_entity);
					ship.range += SHIP_RANGE_UPGRADE;
				}
				upgrade_option.hover = false;
			}
			
		}
	}

	bool itemUsed = false;

	if (click_delay > 0.3f) {
		itemUsed = UISystem::useItemFromInventory(registry, mouse_pos_x, mouse_pos_y, Click::LEFT);
		if (itemUsed) {
			click_delay = 0.0f;
		}
	}

	if (!registry.view<Drag>().empty() && click_delay > 0.3f && !itemUsed) {
		UISystem::dropItem(registry, Click::LEFT);
		UISystem::equip_delay = 0.0f;
		click_delay = 0.0f;
	}
	
	if (player_comp.weapon_cooldown <= 0 && 
		screen_state.current_screen == ScreenState::ScreenType::GAMEPLAY && 
		click_delay > 0.3f) {
			createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity, PROJECTILE_DAMAGE, TEXTURE_ASSET_ID::GOLD_PROJECTILE);
			MusicSystem::playSoundEffect(SFX::SHOOT);
			player_comp.weapon_cooldown = WEAPON_COOLDOWN;
	}
}

void WorldSystem::left_mouse_release() {
    auto& screen_state = registry.get<ScreenState>(screen_entity);

    if (screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI) {
        for (auto entity : registry.view<UpgradeButton>()) {
            auto& upgrade_render = registry.get<RenderRequest>(entity);
            upgrade_render.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE;
        }
    }
}

void WorldSystem::on_mouse_button_pressed(int button, int action, int mods) {
	// on button press
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			debug_printf(DebugType::USER_INPUT, "Mouse clicked at: (%.1f, %.1f)\n", mouse_pos_x, mouse_pos_y);
			left_mouse_click();
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			debug_printf(DebugType::USER_INPUT, "Mouse right clicked at: (%.1f, %.1f)\n", mouse_pos_x, mouse_pos_y);
			right_mouse_click(mods);
		}
	} else if (action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            left_mouse_release();
        }
    }
}


