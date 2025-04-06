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
	quadTree(quadTree),
	physics_system(physics_system),
	flag_system(flag_system)
{
	for (auto i = 0; i < KeyboardState::NUM_STATES; i++) key_state[i] = false;
	player_spawn = {0, 0};
	player_entity = createPlayer(registry, player_spawn);
	ship_entity = createShip(registry, {0, 0});
	main_camera_entity = createCamera(registry, player_entity);

	screen_entity = registry.create();
	registry.emplace<ScreenState>(screen_entity);
	auto& screen_state = registry.get<ScreenState>(screen_entity);
	screen_state.current_screen = ScreenState::ScreenType::TITLE;

	createTitleScreen(registry);
	createPlayerHealthBar(registry);
	createInventory(registry);
	createMinimap(registry);
	createDefaultWeapon(registry);

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

	int w = 2 * WINDOW_WIDTH_PX, h = 2 * WINDOW_HEIGHT_PX;

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
	createUpgradeButton(registry, vec2(w/4 - w/2*0.27f, h/4 - h/2*0.11f), vec2(w/2*0.05f, h/2*0.025f), ButtonOption::Option::SHIP_HEALTH_UPGRADE, TEXTURE_ASSET_ID::RED_BUTTON_PRESSED);
	// blaster
	createUpgradeButton(registry, vec2(w/4 + w/2*0.3f, h/4 - h/2*0.07f), vec2(w/2*0.05f, h/2*0.025f), ButtonOption::Option::SHIP_BLASTER_UPGRADE, TEXTURE_ASSET_ID::RED_BUTTON_PRESSED);
	// range
	createUpgradeButton(registry, vec2(w/4 - w/2*0.26f, h/4 + h/2*0.3f), vec2(w/2*0.05f, h/2*0.025f), ButtonOption::Option::SHIP_RANGE_UPGRADE, TEXTURE_ASSET_ID::RED_BUTTON_PRESSED);
	// fire rate
	createUpgradeButton(registry, vec2(w/4 + w/2*0.33f, h/4 + h/2*0.25f), vec2(w/2*0.05f, h/2*0.025f), ButtonOption::Option::SHIP_FIRERATE_UPGRADE, TEXTURE_ASSET_ID::RED_BUTTON_PRESSED);

	// init all of the text boxes for the tutorial
	textBoxEntities.resize(6);
    vec2 size = vec2(2 * WINDOW_WIDTH_PX / 3, 200);
	int scale = 2;

	std::string tut_0 =
		std::string("... wait...? it looks like you survived the crash... holy s#%t! ") +
		std::string("can you hear us astronaut? welcome to C#42A, AKA planet {1Nova}, the universe's biggest hellhole. ") +
		std::string("it looks like the ship is pretty banged up. you're going to have to repair it to get out of here. ") +
		std::string("can you walk? try using {1'W', 'A', 'S', 'D'} to move. Press {1Enter} to advance dialogue");
    textBoxEntities[0] = createTextBox(registry, vec2(0.f, 200.0f), size, tut_0, scale, vec3(1));

	std::string tut_1 = 
		std::string("great! let's see if the ship's interface is still working. ") + 
		std::string("press {1'F'} near the ship to open and close the {1upgrade UI}");
    textBoxEntities[1] = createTextBox(registry, vec2(0.f, 200.0f), size, tut_1, scale, vec3(1));

	std::string tut_2 =
		std::string("well, it looks like you're not totally screwed after all. ") +
		std::string("we're going to need some {1materials} to repair the ship. go kill some {1alien} scum for their resources! ") +
		std::string("what, you already forgot how to use your blaster? use {1left click} to shoot!");
    textBoxEntities[2] = createTextBox(registry, vec2(0.f, 200.0f), size, tut_2, scale, vec3(1));

	std::string tut_3 =
		std::string("not too shabby, astronaut. Go explore the {Ssavanna}, {Isnow}, {Bbeach}, and {Jjungle} biomes. ") +
		std::string("be careful though; our signals indicate the presence of a {1stronger alien} in each biome. ") +
		std::string("taking those beasts down are sure to net you a hefty reward.");
    textBoxEntities[3] = createTextBox(registry, vec2(0.f, 200.0f), size, tut_3, scale, vec3(1));

	std::string tut_4 =
		std::string("oh, and one more thing. on {1Nova}, each day is only {15 minutes}, so you're only going to ") +
		std::string("get around {1150 seconds} of daylight. it gets really dark, so you'll probably want to camp out ") +
		std::string("by the ship for protection. or don't; it's your funeral...");
	textBoxEntities[4] = createTextBox(registry, vec2(0.f, 200.f), size, tut_4, scale, vec3(1));

	std::string tut_5 =
		std::string("it looks like you found some resources, nice work astronaut! Open your inventory with {1Tab}. ") + 
		std::string("In the inventory, you can pick up your items with {1Right Click} and place them with {1Left Click}. ") +
		std::string("also, try using {1CTRL}, {1Shift}, and {1Alt} with {1Right Click} to pick up different quantities! ") +
		std::string("Press {1Left Click} to interact with an item in the hotbar.");
    textBoxEntities[5] = createTextBox(registry, vec2(0.f, 200.0f), size, tut_5, scale, vec3(1));
    
    // make them all inactive initially
    for (auto entity : textBoxEntities) {
        auto& textData = registry.get<TextData>(entity);
        textData.active = false;
    }
    
    // then set only the first one to active
    auto& firstTextData = registry.get<TextData>(textBoxEntities[0]);
    firstTextData.active = true;
	
	flag_system.reset();

	//---------------------------------------
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	MusicSystem::updateSoundTimers(elapsed_ms_since_last_update);

	click_delay += elapsed_ms_since_last_update / 1000.f;
	UISystem::equip_delay += elapsed_ms_since_last_update / 1000.f;
	auto& screen_state = registry.get<ScreenState>(screen_entity);
	if (screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI) {
		return true;
	}
	if (screen_state.current_screen == ScreenState::ScreenType::TITLE) {
		click_delay = 0;
		return true; 
	}

	screen_state.time += elapsed_ms_since_last_update / 1000.f;
	

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
		player_respawn();
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
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity, PROJECTILE_DAMAGE, PROJECTILE_TIMER, TEXTURE_ASSET_ID::GOLD_PROJECTILE);
						break;
					case BulletType::BLASTER_PROJECTILE:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE*1.5, PROJECTILE_SIZE*1.5), blaster_velocity, BLASTER_PROJ_DAMAGE, PROJECTILE_TIMER, TEXTURE_ASSET_ID::BLASTER_PROJECTILE);
						break;
					case BulletType::MISSLE_PROJECTILE:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE/1.5, PROJECTILE_SIZE*2), missle_velocity, MISSLE_PROJ_DAMAGE, PROJECTILE_TIMER, TEXTURE_ASSET_ID::MISSILE_PROJECTILE);
						break;
					case BulletType::RAILGUN_PROJECTILE:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE/1.5, PROJECTILE_SIZE*2), railgun_velocity, RAILGUN_PROJ_DAMAGE, PROJECTILE_TIMER, TEXTURE_ASSET_ID::RAILGUN_PROJECTILE);
						break;
					case BulletType::SMG_PROJECTILE:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE/1.5, PROJECTILE_SIZE*2), smg_velocity, SMG_PROJ_DAMAGE, PROJECTILE_TIMER, TEXTURE_ASSET_ID::SMG_PROJECTILE);
						break;
					default:
						createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity, PROJECTILE_DAMAGE, PROJECTILE_TIMER, TEXTURE_ASSET_ID::GOLD_PROJECTILE);
						break;
				}
			}
		}
	}

	// TODO: freeze everything if in ship_ui
	
	MapSystem::update_location(registry, player_entity, true);
	if (screen_state.time > (2.0 * M_PI * 60.0)) {
		screen_state.time -= (2.0 * M_PI * 60.0);
	}
	MapSystem::update_weather(registry, player_entity);

	if ((M_PI * 60.0) <= screen_state.time && screen_state.time <= (2.0 * M_PI * 60.0)) {
		// It is night-time now
		MusicSystem::playMusic(NIGHT, -1, 200);
	} else {
		MapSystem::update_background_music(registry, player_entity);
	}
  
	for (auto entity : registry.view<Projectile>()) {
		auto& projectile = registry.get<Projectile>(entity);
		projectile.timer -= elapsed_ms_since_last_update;
		if (projectile.timer <= 0) {
			debug_printf(DebugType::PHYSICS, "Destroying entity (world sys: projectile)\n");

			if (registry.valid(entity)) {
				registry.destroy(entity);
			}
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

	update_upgrade_buttons();
	
	return true;
}

void WorldSystem::player_respawn() {
	auto& screen_state = registry.get<ScreenState>(registry.view<ScreenState>().front());
	screen_state.darken_screen_factor = 0;

	Player& player = registry.get<Player>(player_entity);
	player.health = PLAYER_HEALTH;

	Motion& player_motion = registry.get<Motion>(player_entity);
	player_motion.velocity = {0.f, 0.f};
	player_motion.acceleration = {0.f, 0.f};
	player_motion.position = player_spawn;

	UISystem::updatePlayerHealthBar(registry, PLAYER_HEALTH);
}

void WorldSystem::handleTextBoxes(float elapsed_ms_since_last_update) {
	FlagSystem::TutorialStep currentStep = flag_system.getTutorialStep();

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
		case FlagSystem::TutorialStep::Biome_Read:
			activeIndex = 4;
			break;
        case FlagSystem::TutorialStep::MobKilled:
            activeIndex = 5;
            break;
		default:
			activeIndex = -1;
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
	screen_state.time = 0;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	// auto motions = registry.view<Motion>(entt::exclude<Player, Ship, UIShip, Background, Title, TextData>);	
	auto motions = registry.view<Motion>(entt::exclude<Player, Ship, Background, DeathItems, Grave, ShipWeapon, ShipEngine>);
	for (auto entity : motions) {
		if (registry.any_of<FixedUI>(entity)) {
			if (registry.any_of<Item>(entity)) {
				auto& item = registry.get<Item>(entity);
				if (item.type != Item::Type::DEFAULT_WEAPON &&
					item.type != Item::Type::HOMING_MISSILE &&
					item.type != Item::Type::SHOTGUN) {
					if (registry.valid(entity)) {
						registry.destroy(entity);
					}
				}
			}
			continue;
		}
		else {
			if (registry.valid(entity)) {
				registry.destroy(entity);
			}
		}
	}
	// auto motions = registry.view<Motion>(entt::exclude<Player, Ship, Background, FixedUI, DeathItems, Grave, ShipWeapon>);
	// registry.destroy(motions.begin(), motions.end());
	// vec2& p_pos = registry.get<Motion>(player_entity).position;
	vec2& s_pos = registry.get<Motion>(ship_entity).position;

	// reset ui ship to default ---> not sure if we wanna do this?
	// for (auto ui_ship_entity : registry.view<UIShip>()) {
	// 	auto& ui_ship_render = registry.get<RenderRequest>(ui_ship_entity);
	// 	ui_ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_VERY_DAMAGE;
	// }
	// auto& ship_render = registry.get<RenderRequest>(ship_entity);
	// ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_VERY_DAMAGE;

	MapSystem::populate_ecs(registry, player_spawn, s_pos);
	
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
			for (auto entity : registry.view<TitleOption>()) {
				auto& button_option = registry.get<TitleOption>(entity);
				button_option.hover = false;
			}
		}
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		// int w = 2 * WINDOW_WIDTH_PX, h = 2 * WINDOW_HEIGHT_PX;
		// glfwGetWindowSize(window, &w, &h);

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

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		FlagSystem::TutorialStep ts = flag_system.getTutorialStep();
		if      (ts == FlagSystem::TutorialStep::None)       flag_system.setMoved(true);
		else if (ts == FlagSystem::TutorialStep::Moved)      flag_system.setAccessed(true);
		else if (ts == FlagSystem::TutorialStep::Accessed)   flag_system.setShot(true);
		else if (ts == FlagSystem::TutorialStep::Shot)       flag_system.setBiomeRead(true);
		else if (ts == FlagSystem::TutorialStep::Biome_Read) flag_system.setDone(true);
		else if (ts == FlagSystem::TutorialStep::MobKilled)  flag_system.setDone(true);
	}

	if (key == GLFW_KEY_T && action == GLFW_RELEASE) {
		set_debug(DebugType::TIME, true);
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
		for (auto entity : registry.view<TitleOption>(entt::exclude<Button>)) {
			auto& title_option = registry.get<TitleOption>(entity);
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

	int w = 2 * WINDOW_WIDTH_PX, h = 2 * WINDOW_HEIGHT_PX;

	if (screen_state.current_screen == ScreenState::ScreenType::TITLE) {
		for (auto entity : registry.view<TitleOption>(entt::exclude<Button>)) {
			auto& title_option = registry.get<TitleOption>(entity);
			if (title_option.hover) {
				MusicSystem::playSoundEffect(SFX::SELECT);
				if (title_option.type == TitleOption::Option::PLAY) {
					screen_state.current_screen = ScreenState::ScreenType::GAMEPLAY;
					return;
				}
				else if (title_option.type == TitleOption::Option::EXIT) {
					close_window();
				}
				else if (title_option.type == TitleOption::Option::RESTART) {
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
		// count number of items the player currently has
		// TODO: once blue prints are added into game, make sure that each upgrade requires at least one blueprint
		auto itemsInInv = registry.view<Item, FixedUI>();
		int copperCount = 0;
		int ironCount = 0;
		for (auto entity : itemsInInv) {
			auto& item = registry.get<Item>(entity);

			if (item.type == Item::Type::COPPER) copperCount += item.no;
			if (item.type == Item::Type::IRON) ironCount += item.no;
		}

		for (auto entity : registry.view<UpgradeButton>()) {
			auto& upgrade_option = registry.get<ButtonOption>(entity);
			auto& upgrade_render = registry.get<RenderRequest>(entity);

			if (upgrade_option.hover) {
				// UPGRADE HEALTH ---------------------------------------------------------------------------
				// 5 iron to upgrade health
				if (upgrade_option.type == ButtonOption::Option::SHIP_HEALTH_UPGRADE && ironCount >= SHIP_HEALTH_UPGRADE_IRON) {
					for (auto ui_ship_entity : registry.view<UIShip>()) {
						auto& ui_ship_render = registry.get<RenderRequest>(ui_ship_entity);

						if (ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_FULL_HP) break;

						upgrade_render.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_PRESSED;
						MusicSystem::playSoundEffect(SFX::SELECT);
						
						// update inventory
						ship_upgrade_inventory(SHIP_HEALTH_UPGRADE_IRON, 0);

						if (ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_VERY_DAMAGE) {
							ui_ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_DAMAGE;
						} else if (ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_DAMAGE) {
							ui_ship_render.used_texture = TEXTURE_ASSET_ID::SHIP_SLIGHT_DAMAGE;
						} else if (ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_SLIGHT_DAMAGE) {
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

				// UPGRADE BLASTER ---------------------------------------------------------------------------
				// 3 copper, 3 iron to upgrade blasters
				// smg --> missles --> blaster --> railgun
				if (upgrade_option.type == ButtonOption::Option::SHIP_BLASTER_UPGRADE && ironCount >= SHIP_WEAPON_UPGRADE_IRON && copperCount >= SHIP_WEAPON_UPGRADE_COPPER) {
					for (auto ui_ship_weapon_entity : registry.view<UIShipWeapon>()) {
						auto& ui_ship_weapon = registry.get<UIShipWeapon>(ui_ship_weapon_entity);
						auto& ui_ship_render = registry.get<RenderRequest>(ui_ship_weapon_entity);
						vec2& ship_pos = registry.get<Motion>(ship_entity).position;

						if (ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_RAILGUN_WEAPON) break;

						upgrade_render.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_PRESSED;
						MusicSystem::playSoundEffect(SFX::SELECT);
						
						// update inventory
						ship_upgrade_inventory(SHIP_HEALTH_UPGRADE_IRON, SHIP_WEAPON_UPGRADE_COPPER);

						if (ui_ship_weapon.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_SMG_WEAPON) {
							// change to missles

							if (registry.valid(ui_ship_weapon_entity)) {
								registry.destroy(ui_ship_weapon_entity);
							}
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

							if (!shipWeapon.empty()) {
								registry.destroy(shipWeapon.begin(), shipWeapon.end());
							}
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

							if (registry.valid(ui_ship_weapon_entity)) {
								registry.destroy(ui_ship_weapon_entity);
							}
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

							if (!shipWeapon.empty()) {
								registry.destroy(shipWeapon.begin(), shipWeapon.end());
							}
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

							if (registry.valid(ui_ship_weapon_entity)) {
								registry.destroy(ui_ship_weapon_entity);
							}
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

							if (!shipWeapon.empty()) {
								registry.destroy(shipWeapon.begin(), shipWeapon.end());
							}
							createShipWeapon( registry, 
								vec2(ship_pos.x, ship_pos.y),
								vec2(110, 100),
								vec2(24.f, 48.f), 
								vec2(336.f, 48.f),
								{0, 3}, 
								7 );
							
							// change the bullet type
							bulletType = BulletType::RAILGUN_PROJECTILE;

						} else if (!ui_ship_weapon.active) {
							// create a new weapon (start as smg)
							if (registry.valid(ui_ship_weapon_entity)) {
								registry.destroy(ui_ship_weapon_entity);
							}
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

							if (!shipWeapon.empty()) {
								registry.destroy(shipWeapon.begin(), shipWeapon.end());
							}
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

				// UPGRADE FIRE RATE ---------------------------------------------------------------------------
				// smg engine --> missles engine --> blaster engine --> railgun engine
				if (upgrade_option.type == ButtonOption::Option::SHIP_FIRERATE_UPGRADE && ironCount >= SHIP_FIRERATE_UPGRADE_IRON && copperCount >= SHIP_FIRERATE_UPGRADE_COPPER) {
					for (auto ui_ship_engine_entity : registry.view<UIShipEngine>()) {
						auto& ui_ship_engine = registry.get<UIShipEngine>(ui_ship_engine_entity);
						auto& ui_ship_render = registry.get<RenderRequest>(ui_ship_engine_entity);
						vec2& ship_pos = registry.get<Motion>(ship_entity).position;
						auto& ship = registry.get<Ship>(ship_entity);

						if (ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_RAILGUN_ENGINE) break;

						upgrade_render.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_PRESSED;
						MusicSystem::playSoundEffect(SFX::SELECT);

						// update inventory
						ship_upgrade_inventory(SHIP_FIRERATE_UPGRADE_IRON, SHIP_FIRERATE_UPGRADE_COPPER);

						if (ui_ship_engine.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_SMG_ENGINE) {
							// change to missles engine
							if (registry.valid(ui_ship_engine_entity)) {
								registry.destroy(ui_ship_engine_entity);
							}
							entt::entity chosenEngine = createUIShipEngine(registry, 
								vec2(w/4, h/4 + h/2/37.3),
								vec2(w/6 - w/2/6.7, w/6 - w/2/6.7),
								10);
							auto& currEngine = registry.get<UIShipEngine>(chosenEngine);
							currEngine.active = true;

							// update gameplay ship
							auto shipEngine = registry.view<ShipEngine>();
							if (!shipEngine.empty()) {
								registry.destroy(shipEngine.begin(), shipEngine.end());
							}
							createShipEngine(registry, vec2(ship_pos.x - 13.0f, ship_pos.y) , vec2(240.0f - 125.0f, 137.5f - 35.0f), 10);

							ship.timer += SHIP_TIMER_UPGRADE;
						} else if (ui_ship_engine.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_MISSLE_ENGINE) {
							// change to blaster engine
							if (registry.valid(ui_ship_engine_entity)) {
								registry.destroy(ui_ship_engine_entity);
							}
							entt::entity chosenEngine = createUIShipEngine(registry, 
								vec2(w/4, h/4 + h/2/37.3),
								vec2(w/6 - w/2/6.7, w/6 - w/2/6.7),
								9);
							auto& currEngine = registry.get<UIShipEngine>(chosenEngine);
							currEngine.active = true;

							// update gameplay ship
							auto shipEngine = registry.view<ShipEngine>();
							if (!shipEngine.empty()) {
								registry.destroy(shipEngine.begin(), shipEngine.end());
							}
							createShipEngine(registry, vec2(ship_pos.x - 13.0f, ship_pos.y) , vec2(240.0f - 125.0f, 137.5f - 35.0f), 9);

							ship.timer += SHIP_TIMER_UPGRADE;
						} else if (ui_ship_engine.active && ui_ship_render.used_texture == TEXTURE_ASSET_ID::SHIP_BLASTER_ENGINE) {
							// change to railgun engine
							if (registry.valid(ui_ship_engine_entity)) {
								registry.destroy(ui_ship_engine_entity);
							}
							entt::entity chosenEngine = createUIShipEngine(registry, 
								vec2(w/4, h/4 + h/2/56),
								vec2(w/6 - w/2/6.7, w/6 - w/2/6.7),
								11);
							auto& currEngine = registry.get<UIShipEngine>(chosenEngine);
							currEngine.active = true;

							// update gameplay ship
							auto shipEngine = registry.view<ShipEngine>();
							if (!shipEngine.empty()) {
								registry.destroy(shipEngine.begin(), shipEngine.end());
							}
							createShipEngine(registry, vec2(ship_pos.x - 10.0f, ship_pos.y) , vec2(240.0f - 125.0f, 137.5f - 35.0f), 11);

							ship.timer += SHIP_TIMER_UPGRADE;
						} else if (!ui_ship_engine.active) {
							// create a new weapon (start as smg engine)
							if (registry.valid(ui_ship_engine_entity)) {
								registry.destroy(ui_ship_engine_entity);
							}
							entt::entity chosenEngine = createUIShipEngine(registry, 
								vec2(w/4, h/4 + h/2/112),
								vec2(w/6 - w/2/6.7, w/6 - w/2/6.7),
								12);
							auto& currEngine = registry.get<UIShipEngine>(chosenEngine);
							currEngine.active = true;
							
							// update gameplay ship
							auto shipEngine = registry.view<ShipEngine>();
							if (!shipEngine.empty()) {
								registry.destroy(shipEngine.begin(), shipEngine.end());
							}
							createShipEngine(registry, vec2(ship_pos.x - 13.0f, ship_pos.y) , vec2(240.0f - 125.0f, 137.5f - 35.0f), 12);

							ship.timer += SHIP_TIMER_UPGRADE;
						}
					}
				}

				// UPGRADE RANGE ---------------------------------------------------------------------------
				if (upgrade_option.type == ButtonOption::Option::SHIP_RANGE_UPGRADE && ironCount >= SHIP_RANGE_UPGRADE_IRON) {
					upgrade_render.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_PRESSED;
					MusicSystem::playSoundEffect(SFX::SELECT);
					
					// update inventory
					ship_upgrade_inventory(SHIP_RANGE_UPGRADE_IRON, 0);

					auto& ship = registry.get<Ship>(ship_entity);
					ship.range += SHIP_RANGE_UPGRADE;
				}
				// upgrade_option.hover = false;
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
		UISystem::dropItem(registry);
		UISystem::equip_delay = 0.0f;
		click_delay = 0.0f;
	}
	
	if (player_comp.weapon_cooldown <= 0 && 
		screen_state.current_screen == ScreenState::ScreenType::GAMEPLAY && 
		click_delay > 0.3f) {
		auto& weapon = registry.get<Item>(registry.get<InventorySlot>(*registry.view<ActiveSlot>().begin()).item);
		flag_system.setShot(true);
		if (weapon.type == Item::Type::DEFAULT_WEAPON) {
			createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity, PROJECTILE_DAMAGE, PROJECTILE_TIMER, TEXTURE_ASSET_ID::GOLD_PROJECTILE);
			player_comp.weapon_cooldown = WEAPON_COOLDOWN;
			MusicSystem::playSoundEffect(SFX::SHOOT);
		}
		else if (weapon.type == Item::Type::HOMING_MISSILE) {
			auto missile_entity = createProjectile(registry, player_motion.position, 2.5f * vec2(PROJECTILE_SIZE / 2.8, PROJECTILE_SIZE), velocity, PROJECTILE_DAMAGE * 2, PROJECTILE_TIMER, TEXTURE_ASSET_ID::MISSILE_PROJECTILE);
			auto& player_pos = registry.get<Motion>(*registry.view<Player>().begin()).position;
			float x = mouse_pos_x - WINDOW_WIDTH_PX / 2 + player_pos.x;
			float y = mouse_pos_y - WINDOW_HEIGHT_PX / 2 + player_pos.y;
			findNearestTarget(registry, missile_entity, x, y);
			player_comp.weapon_cooldown = WEAPON_COOLDOWN * 4;
			MusicSystem::playSoundEffect(SFX::MISSILE);
		}
		else if (weapon.type == Item::Type::SHOTGUN) {
			createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity, PROJECTILE_DAMAGE, 250, TEXTURE_ASSET_ID::SHOTGUN_PROJECTILE);
			createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), vec2(velocity.x * 0.966 - velocity.y * 0.259, velocity.y * 0.966 + velocity.x * 0.259), PROJECTILE_DAMAGE, 250, TEXTURE_ASSET_ID::SHOTGUN_PROJECTILE);
			createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), vec2(velocity.x * 0.966 + velocity.y * 0.259, velocity.y * 0.966 - velocity.x * 0.259), PROJECTILE_DAMAGE, 250, TEXTURE_ASSET_ID::SHOTGUN_PROJECTILE);
			createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), vec2(velocity.x * 0.866 - velocity.y * 0.5, velocity.y * 0.866 + velocity.x * 0.5), PROJECTILE_DAMAGE, 250, TEXTURE_ASSET_ID::SHOTGUN_PROJECTILE);
			createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), vec2(velocity.x * 0.866 + velocity.y * 0.5, velocity.y * 0.866 - velocity.x * 0.5), PROJECTILE_DAMAGE, 250, TEXTURE_ASSET_ID::SHOTGUN_PROJECTILE);
			player_comp.weapon_cooldown = WEAPON_COOLDOWN * 2;
			MusicSystem::playSoundEffect(SFX::SHOTGUN);
		}
	}
}

void WorldSystem::left_mouse_release() {
    auto& screen_state = registry.get<ScreenState>(screen_entity);

    if (screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI) {
        for (auto entity : registry.view<UpgradeButton>()) {
            auto& upgrade_render = registry.get<RenderRequest>(entity);
			// if (upgrade_render.used_texture != TEXTURE_ASSET_ID::RED_BUTTON_PRESSED) {
			// 	upgrade_render.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE;
			// }
			update_upgrade_buttons();
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

// update inventory for ship upgrades
void WorldSystem::ship_upgrade_inventory(int ironCount, int copperCount) {
	int numIronLeftToUse = ironCount;
	int numCopperLeftToUse = copperCount;

	for (auto inventory_slot_entity : registry.get<Inventory>(*registry.view<Inventory>().begin()).slots) {
		auto& inventory_slot = registry.get<InventorySlot>(inventory_slot_entity);
		if (!inventory_slot.hasItem) continue;
		auto& item = registry.get<Item>(inventory_slot.item);

		if (item.type == Item::Type::IRON) {
			if (item.no <= numIronLeftToUse) {
				numIronLeftToUse -= item.no;
				inventory_slot.hasItem = false;
				if (registry.valid(inventory_slot.item)) {
					registry.destroy(inventory_slot.item);
				}
			} else { 
				item.no -= numIronLeftToUse;
				numIronLeftToUse = 0;
			}
		}

		if (item.type == Item::Type::COPPER) {
			if (item.no <= numCopperLeftToUse) {
				numCopperLeftToUse -= item.no;
				inventory_slot.hasItem = false;
				if (registry.valid(inventory_slot.item)) {
					registry.destroy(inventory_slot.item);
				}
			} else { 
				item.no -= numCopperLeftToUse;
				numCopperLeftToUse = 0;
			}
		}

		if (numCopperLeftToUse == 0 && numIronLeftToUse == 0) {
			break;
		}
	}
}


void WorldSystem::update_upgrade_buttons() {
	// get num of each item needed for upgrades
	auto itemsInInv = registry.view<Item, FixedUI>();
	int copperCount = 0;
	int ironCount = 0;
	for (auto entity : itemsInInv) {
		auto& item = registry.get<Item>(entity);
		
		if (item.type == Item::Type::COPPER) copperCount += item.no;
		if (item.type == Item::Type::IRON) ironCount += item.no;
	}

	auto ui_ship_weapon_entity = registry.view<UIShipWeapon>().front();
	auto& ui_ship_weapon_render = registry.get<RenderRequest>(ui_ship_weapon_entity);

	auto ui_ship_engine_entity = registry.view<UIShipEngine>().front();
	auto& ui_ship_engine_render = registry.get<RenderRequest>(ui_ship_engine_entity);

	auto& ship = registry.get<Ship>(ship_entity);
	auto& ship_render = registry.get<RenderRequest>(ship_entity);
	
	// if have enough resources, change the buttons in the UI to green (upgradeable)
	auto upgradeButtons = registry.view<UpgradeButton, ButtonOption>();
	for (auto entity : upgradeButtons) {
		auto& buttonOption = registry.get<ButtonOption>(entity);
		auto& buttonRenderRequest = registry.get<RenderRequest>(entity);
		auto& upgradeButton = registry.get<UpgradeButton>(entity);
		
		if (buttonOption.type == ButtonOption::Option::SHIP_HEALTH_UPGRADE && ironCount >= SHIP_HEALTH_UPGRADE_IRON && ship_render.used_texture != TEXTURE_ASSET_ID::SHIP_FULL_HP) {
			buttonRenderRequest.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE;
			upgradeButton.missingResources = false;
			upgradeButton.missingResourcesText = std::to_string(SHIP_HEALTH_UPGRADE_IRON) + " iron";
		} else if (buttonOption.type == ButtonOption::Option::SHIP_HEALTH_UPGRADE) {
			buttonRenderRequest.used_texture = TEXTURE_ASSET_ID::RED_BUTTON_PRESSED;
			upgradeButton.missingResources = true;
			upgradeButton.missingResourcesText = std::to_string(SHIP_HEALTH_UPGRADE_IRON) + " iron";
		}
		
		if (buttonOption.type == ButtonOption::Option::SHIP_BLASTER_UPGRADE && ironCount >= SHIP_WEAPON_UPGRADE_IRON && copperCount >= SHIP_WEAPON_UPGRADE_COPPER && ui_ship_weapon_render.used_texture != TEXTURE_ASSET_ID::SHIP_RAILGUN_WEAPON) {
			buttonRenderRequest.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE;
			upgradeButton.missingResources = false;
			upgradeButton.missingResourcesText = std::to_string(SHIP_WEAPON_UPGRADE_IRON) + " iron, " + std::to_string(SHIP_WEAPON_UPGRADE_COPPER) + " copper";
		} else if (buttonOption.type == ButtonOption::Option::SHIP_BLASTER_UPGRADE) {
			buttonRenderRequest.used_texture = TEXTURE_ASSET_ID::RED_BUTTON_PRESSED;
			upgradeButton.missingResources = true;
			upgradeButton.missingResourcesText = std::to_string(SHIP_WEAPON_UPGRADE_IRON) + " iron, " + std::to_string(SHIP_WEAPON_UPGRADE_COPPER) + " copper";
		}
		
		if (buttonOption.type == ButtonOption::Option::SHIP_FIRERATE_UPGRADE && ironCount >= SHIP_FIRERATE_UPGRADE_IRON && copperCount >= SHIP_FIRERATE_UPGRADE_COPPER && ui_ship_engine_render.used_texture != TEXTURE_ASSET_ID::SHIP_RAILGUN_ENGINE) {
			buttonRenderRequest.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE;
			upgradeButton.missingResources = false;
			upgradeButton.missingResourcesText = std::to_string(SHIP_FIRERATE_UPGRADE_IRON) + " iron, " + std::to_string(SHIP_FIRERATE_UPGRADE_COPPER) + " copper";
		} else if (buttonOption.type == ButtonOption::Option::SHIP_FIRERATE_UPGRADE) {
			buttonRenderRequest.used_texture = TEXTURE_ASSET_ID::RED_BUTTON_PRESSED;
			upgradeButton.missingResources = true;
			upgradeButton.missingResourcesText = std::to_string(SHIP_FIRERATE_UPGRADE_IRON) + " iron, " + std::to_string(SHIP_FIRERATE_UPGRADE_COPPER) + " copper";
		}
		
		if (buttonOption.type == ButtonOption::Option::SHIP_RANGE_UPGRADE && ironCount >= SHIP_RANGE_UPGRADE_IRON && ship.range < SHIP_MAX_RANGE) {
			buttonRenderRequest.used_texture = TEXTURE_ASSET_ID::GREEN_BUTTON_ACTIVE;
			upgradeButton.missingResources = false;
			upgradeButton.missingResourcesText = std::to_string(SHIP_RANGE_UPGRADE_IRON) + " iron";
		} else if (buttonOption.type == ButtonOption::Option::SHIP_RANGE_UPGRADE) {
			buttonRenderRequest.used_texture = TEXTURE_ASSET_ID::RED_BUTTON_PRESSED;
			upgradeButton.missingResources = true;
			upgradeButton.missingResourcesText = std::to_string(SHIP_RANGE_UPGRADE_IRON) + " iron";
		}
	}
}

