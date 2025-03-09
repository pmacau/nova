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
WorldSystem::WorldSystem(entt::registry& reg, PhysicsSystem& physics_system) :
	registry(reg),
	physics_system(physics_system)
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

	// seeding rng with random device
	rng = std::default_random_engine(std::random_device()());


	// init all the ui ships to use
	createUIShip(registry, vec2(WINDOW_WIDTH_PX/2 - 300, WINDOW_HEIGHT_PX/2 - 25), vec2(1.5f, 3.0f), 6);

	createUIShip(registry, vec2(WINDOW_WIDTH_PX/2 - 10, WINDOW_HEIGHT_PX/2 - 145), vec2(1.5f, 1.5f), 2);
	createUIShip(registry, vec2(WINDOW_WIDTH_PX/2 - 10, WINDOW_HEIGHT_PX/2 + 100), vec2(1.5f, 1.5f), 3);

	createUIShip(registry, vec2(WINDOW_WIDTH_PX/2 + 250, WINDOW_HEIGHT_PX/2 - 155), vec2(1.5f, 1.5f), 5);
	createUIShip(registry, vec2(WINDOW_WIDTH_PX/2 + 250, WINDOW_HEIGHT_PX/2 + 10), vec2(1.5f, 1.5f), 1);
	createUIShip(registry, vec2(WINDOW_WIDTH_PX/2 + 250, WINDOW_HEIGHT_PX/2 + 190), vec2(1.5f, 1.5f), 4);
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

    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	projectile_shooting_delay = projectile_shooting_delay + elapsed_ms_since_last_update / 1000.f;
	auto screen_state = registry.get<ScreenState>(screen_entity);
	if (screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI) {
		return true;
	}
	if (screen_state.current_screen == ScreenState::ScreenType::TITLE) {
		projectile_shooting_delay = 0;
		return true; 
	}
	

	auto player = registry.get<Player>(player_entity);
	if (player.health <= 0) {
		debug_printf(DebugType::WORLD, "Game over; restarting game now...\n");
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
		ship.timer = SHIP_TIMER_MS;
		
		for (auto entity : mobs) {
			auto motion = registry.get<Motion>(entity);
			auto shipMotion = registry.get<Motion>(ship_entity);

			glm::vec2 shipPos = glm::vec2(shipMotion.position.x, shipMotion.position.y);
			glm::vec2 enemyPos = glm::vec2(motion.position.x, motion.position.y);

			if (glm::distance(shipPos, enemyPos) <= ship.range) {
				vec2 direction = normalize(enemyPos - shipPos);
				vec2 velocity = direction * PROJECTILE_SPEED;
				createProjectile(registry, shipMotion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity);
			}
		}
	}

	// TODO: freeze everything if in ship_ui
	
	
	

	// TODO: move player out-of-bounds script
	MapSystem::update_location(registry, player_entity);

	// auto& player_motion = registry.get<Motion>(player_entity);
	// int tile_x = std::round((player_motion.position.x) / 16.f);
	// int tile_y = std::round((player_motion.position.y + player_motion.scale.y / 2) / 16.f);
	// int former_x = std::round((player_motion.formerPosition.x) / 16.f);
	// int former_y = std::round((player_motion.formerPosition.y + player_motion.scale.y / 2) / 16.f);

	// auto valid_tile = [this](int tile_x, int tile_y) {
	// 	bool in_bounds = (tile_x >= 0 && tile_y >= 0 && tile_x < MAP_TILE_WIDTH && tile_y < MAP_TILE_HEIGHT);
	// 	if (in_bounds) {
	// 		bool in_water = gameMap[tile_y][tile_x] == 0;
	// 		return !in_water;
	// 	}
	// 	return false;
	// };

	// if (!valid_tile(tile_x, tile_y)) {
	// 	if (valid_tile(tile_x, former_y)) {
	// 		player_motion.position = {player_motion.position.x, player_motion.formerPosition.y};
	// 	} else if (valid_tile(former_x, tile_y)) {
	// 		player_motion.position = {player_motion.formerPosition.x, player_motion.position.y};
	// 	} else {
	// 		player_motion.position = player_motion.formerPosition;
	// 	}
	// }
  
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

	return true;
}

void WorldSystem::player_respawn() {
	Player& player = registry.get<Player>(player_entity);
	player.health = PLAYER_HEALTH;

	Motion& player_motion = registry.get<Motion>(player_entity);

	player_motion.velocity = {0.f, 0.f};
	player_motion.acceleration = {0.f, 0.f};
	player_motion.formerPosition = player_motion.position;
	UISystem::updatePlayerHealthBar(registry, PLAYER_HEALTH);
}


// Reset the world state to its initial state
void WorldSystem::restart_game() {
	debug_printf(DebugType::WORLD, "Restarting...\n");

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	auto motions = registry.view<Motion>(entt::exclude<Player, Ship, UIShip, Background, Title>);	
	registry.destroy(motions.begin(), motions.end());
	debug_printf(DebugType::PHYSICS, "Destroying entity (world sys: restart_game)\n");

	vec2& p_pos = registry.get<Motion>(player_entity).position;
	vec2& s_pos = registry.get<Motion>(ship_entity).position;

	MapSystem::populate_ecs(registry, p_pos, s_pos);

	player_respawn();
	createPlayerHealthBar(registry, p_pos);
	createInventory(registry);
	// reset the screen
	auto screen_state = registry.get<ScreenState>(screen_entity);
	screen_state.darken_screen_factor = 0;
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
	// // Debugging - not used in A1, but left intact for the debug lines
	// if (key == GLFW_KEY_D) {
	// 	if (action == GLFW_RELEASE) {
	// 		if (debugging.in_debug_mode) {
	// 			debugging.in_debug_mode = false;
	// 		}
	// 		else {
	// 			debugging.in_debug_mode = true;
	// if (key == GLFW_KEY_P) {
	// 	auto debugView = registry.view<Debug>();
	// 	if (debugView.empty()) {
	// 		registry.emplace<Debug>(player_entity);
	// 	}
	// 	else {
	// 		for (auto entity : debugView) {
	// 			std::cout << "Removing debug" << std::endl;
	// 			registry.remove<Debug>(entity);
	// 		}
	// 	}
	// }

	// E to toggle opening/closign ship ui
	if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
        if (screen_state.current_screen == ScreenState::ScreenType::GAMEPLAY) {
            auto& player_motion = registry.get<Motion>(player_entity);
            auto& ship_motion = registry.get<Motion>(ship_entity);

			player_motion.velocity.x = 0;
			player_motion.velocity.y = 0;

            float distance_to_ship = glm::distance(player_motion.position, ship_motion.position);
            if (distance_to_ship < 150.0f) {
				debug_printf(DebugType::USER_INPUT, "Opening Ship Upgrade UI\n");
                screen_state.current_screen = ScreenState::ScreenType::SHIP_UPGRADE_UI;
            }
        } else if (screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI) {
			debug_printf(DebugType::USER_INPUT, "Closing Ship Upgrade UI\n");
            screen_state.current_screen = ScreenState::ScreenType::GAMEPLAY;
        }
    }

	// TODO: testing sound system. remove this later
	if (key == GLFW_KEY_1) MusicSystem::playMusic(Music::FOREST, -1, 200);
	if (key == GLFW_KEY_2) MusicSystem::playMusic(Music::BEACH, -1, 200);
	if (key == GLFW_KEY_3) MusicSystem::playMusic(Music::SNOWLANDS, -1, 200);
	if (key == GLFW_KEY_4) MusicSystem::playMusic(Music::SAVANNA, -1, 200);
	if (key == GLFW_KEY_5) MusicSystem::playMusic(Music::OCEAN, -1, 200);
	if (key == GLFW_KEY_6) MusicSystem::playMusic(Music::JUNGLE, -1, 200);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {

	// record the current mouse position
	mouse_pos_x = mouse_position.x;
	mouse_pos_y = mouse_position.y;
	auto& screen_state = registry.get<ScreenState>(screen_entity);
	if (screen_state.current_screen == ScreenState::ScreenType::TITLE) {
		for (auto entity : registry.view<TitleOption>()) {
			auto& title_option = registry.get<TitleOption>(entity);
			title_option.hover = abs(mouse_pos_x - title_option.position.x) <= title_option.size.x / 2 &&
				abs(mouse_pos_y - title_option.position.y) <= title_option.size.y / 2;
		}
	}
}

void WorldSystem::left_mouse_click() {
	auto& player_motion = registry.get<Motion>(player_entity);
	vec2 player_to_mouse_direction = vec2(mouse_pos_x, mouse_pos_y) - vec2(WINDOW_WIDTH_PX / 2, WINDOW_HEIGHT_PX / 2);
	vec2 direction = normalize(player_to_mouse_direction); // player position is always at (0, 0) in camera space
	vec2 velocity = direction * PROJECTILE_SPEED;

	auto& player_comp = registry.get<Player>(player_entity);
	auto& screen_state = registry.get<ScreenState>(screen_entity);
	if (screen_state.current_screen == ScreenState::ScreenType::TITLE) {
		for (auto entity : registry.view<TitleOption>()) {
			auto& title_option = registry.get<TitleOption>(entity);
			if (title_option.hover) {
				if (title_option.type == TitleOption::Option::PLAY) {
					screen_state.current_screen = ScreenState::ScreenType::GAMEPLAY;
					return;
				}
				else if (title_option.type == TitleOption::Option::EXIT) {
					close_window();
				}
			}
			
		}
	}

	if (UISystem::useItemFromInventory(registry, mouse_pos_x, mouse_pos_y)) {
		projectile_shooting_delay = 0.0f;
	}
	
	if (player_comp.weapon_cooldown <= 0 && 
		screen_state.current_screen == ScreenState::ScreenType::GAMEPLAY && 
		projectile_shooting_delay > 0.5f) {
			createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity);
			MusicSystem::playSoundEffect(SFX::SHOOT);
			player_comp.weapon_cooldown = WEAPON_COOLDOWN;
	}
}


void WorldSystem::on_mouse_button_pressed(int button, int action, int mods) {
	// on button press
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			debug_printf(DebugType::USER_INPUT, "Mouse clicked at: (%.1f, %.1f)\n", mouse_pos_x, mouse_pos_y);
			left_mouse_click();
		}
	}
}


