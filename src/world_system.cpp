// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "tinyECS/components.hpp"
#include "util/file_loader.hpp"
#include "ui_system.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

// create the world
WorldSystem::WorldSystem(entt::registry& reg, PhysicsSystem& physics_system) :
	registry(reg),
	physics_system(physics_system),
	next_invader_spawn(0),
	invader_spawn_rate_ms(INVADER_SPAWN_RATE_MS),
	points(0)
{

	for (auto i = 0; i < KeyboardState::NUM_STATES; i++) key_state[i] = false;

	// Create background map entity
	auto entity = reg.create();
	reg.emplace<Background>(entity);
	
	auto& sprite = reg.emplace<Sprite>(entity);
	sprite.coord = {0.0f, 0.0f};
	sprite.dims = {16.f * 199.f, 16.f * 199.f};
	sprite.sheet_dims = {16.f * 199.f, 16.f * 199.f};

	auto& motion = reg.emplace<Motion>(entity);
	motion.position = {8.f * 199.f, 8.f * 199.f}; // make top-left corner of map at 0,0?
	motion.scale = {16.f * 199.f, 16.f * 199.f};

	auto& renderRequest = reg.emplace<RenderRequest>(entity);
	renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
	renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	renderRequest.used_texture = TEXTURE_ASSET_ID::MAP_BACKGROUND;

	gameMap = loadBinaryMap(map_path("map.bin"), 200, 200);
	for (int i = 0; i < 200; i++) {
		for (int j = 0; j < 200; j++) {
			if (gameMap[i][j] == 3) {
				spawnX = j * 16;
				spawnY = i * 16;
			}
		}
	}

	player_entity = createPlayer(registry, vec2(spawnX, spawnY));
	ship_entity = createShip(registry, vec2(spawnX, spawnY - 200));
	main_camera_entity = createCamera(registry, player_entity);

	// seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (chicken_dead_sound != nullptr)
		Mix_FreeChunk(chicken_dead_sound);
	if (chicken_eat_sound != nullptr)
		Mix_FreeChunk(chicken_eat_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear();
	// registry.clear_all_components();

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

bool WorldSystem::start_and_load_sounds() {
	
	//////////////////////////////////////
	// Loading music and sounds with SDL

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	chicken_dead_sound = Mix_LoadWAV(audio_path("chicken_dead.wav").c_str());
	chicken_eat_sound = Mix_LoadWAV(audio_path("chicken_eat.wav").c_str());

	if (background_music == nullptr || chicken_dead_sound == nullptr || chicken_eat_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("chicken_dead.wav").c_str(),
			audio_path("chicken_eat.wav").c_str());
		return false;
	}

	return true;
}

void WorldSystem::init() {
	// start playing background music indefinitely
	std::cout << "Starting music..." << std::endl;
	Mix_PlayMusic(background_music, -1);
	// Set all states to default

    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	auto player = registry.get<Player>(player_entity);
	if (player.health <= 0) {
		printf("[GAME OVER] restarting game now...\n");
		restart_game();
	}

	auto& player_motion_debug = registry.get<Motion>(player_entity);
	// printf("The player position is: (%f, %f)\n", player_motion_debug.position.x, player_motion_debug.position.y);
	
	// TODO: refactor this logic to be more reusable/modular i.e. make a helper to update player speed based on key state
	/*auto updatePlayerVelocity = [this]() {
		auto& motion = registry.get<Motion>(player_entity);
	    motion.velocity.y = (!key_state[KeyboardState::UP]) ? (key_state[KeyboardState::DOWN] ? PLAYER_SPEED: 0.0f) : -PLAYER_SPEED;
		motion.velocity.x = (!key_state[KeyboardState::LEFT]) ? (key_state[KeyboardState::RIGHT] ? PLAYER_SPEED: 0.0f) : -PLAYER_SPEED;

		if      (key_state[KeyboardState::UP]    && key_state[KeyboardState::DOWN])  motion.velocity.y = 0.0f;
		else if (key_state[KeyboardState::LEFT]  && key_state[KeyboardState::RIGHT]) motion.velocity.x = 0.0f;
		else if (key_state[KeyboardState::LEFT]  && key_state[KeyboardState::UP])    motion.velocity = PLAYER_SPEED * vec2(-0.7071f, -0.7071f);
		else if (key_state[KeyboardState::LEFT]  && key_state[KeyboardState::DOWN])  motion.velocity = PLAYER_SPEED * vec2(-0.7071f,  0.7071f);
		else if (key_state[KeyboardState::RIGHT] && key_state[KeyboardState::UP])    motion.velocity = PLAYER_SPEED * vec2( 0.7071f, -0.7071f);
		else if (key_state[KeyboardState::RIGHT] && key_state[KeyboardState::DOWN])  motion.velocity = PLAYER_SPEED * vec2( 0.7071f,  0.7071f);
	};
	updatePlayerVelocity(); */
	InputState i; 
	if (key_state[KeyboardState::UP]) {
		i.up = true;
	}
	if (key_state[KeyboardState::DOWN]) {
		i.down = true;
	}
	if (key_state[KeyboardState::LEFT]) {
		i.left = true;
	}
	if (key_state[KeyboardState::RIGHT]) {
		i.right = true;
	}
	physics_system.updatePlayerVelocity(i);


	// TODO: move direction system
	auto dir_view = registry.view<Motion, Sprite>();
	for (auto& entity : dir_view) {
		auto& motion = registry.get<Motion>(entity);
		auto& sprite = registry.get<Sprite>(entity);

		if (length(motion.velocity) > 0.0f) {
			vec2 velo = motion.velocity;
			float x_scale = abs(motion.scale.x);

			if (abs(velo.y) > 0) {
				sprite.coord.x = (velo.y > 0) ? sprite.down_row : sprite.up_row;
				motion.scale.x = x_scale;
			}

			if (abs(velo.x) > 0) {
				sprite.coord.x = sprite.right_row;
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
			p_sprite.coord.x = 1.f;
			p_motion.scale.x = (velo.x < 0) ? -1.f * x_scale : x_scale;
		}

		if (abs(velo.y) > 0) {
			p_sprite.coord.x = (velo.y > 0) ? 0.f : 2.f;
			p_motion.scale.x = x_scale;
		}
	}

	// TODO: move this animation system
	auto animations = registry.view<Animation, Sprite, Motion>();
	for (auto entity : animations) {
		auto& sprite = registry.get<Sprite>(entity);
		auto& animation = registry.get<Animation>(entity);
		auto& motion = registry.get<Motion>(entity);

		animation.frameTime += elapsed_ms_since_last_update;
		if (animation.frameTime >= animation.frameDuration) {
			if (length(motion.velocity) <= 0.5f) {
				sprite.coord.y = 0;
			} else {
				int numFrames = (int) (sprite.sheet_dims.x / sprite.dims.x);
				sprite.coord.y = ((int) (sprite.coord.y + 1)) % numFrames;
			}
			animation.frameTime = 0.0f;
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

	// TODO: move player out-of-bounds script
	//       (probably to physics system, but only world system knows about gameMap)

	auto& player_motion = registry.get<Motion>(player_entity);
	int tile_x = std::round((player_motion.position.x) / 16.f);
	int tile_y = std::round((player_motion.position.y + player_motion.scale.y / 2) / 16.f);
	int former_x = std::round((player_motion.formerPosition.x) / 16.f);
	int former_y = std::round((player_motion.formerPosition.y + player_motion.scale.y / 2) / 16.f);

	auto valid_tile = [this](int tile_x, int tile_y) {
		bool in_bounds = (tile_x >= 0 && tile_y >= 0 && tile_x < 200 && tile_y < 200);
		bool in_water = gameMap[tile_y][tile_x] == 0;
		return in_bounds && !in_water;
	};

	if (!valid_tile(tile_x, tile_y)) {
		if (valid_tile(tile_x, former_y)) {
			player_motion.position = {player_motion.position.x, player_motion.formerPosition.y};
		} else if (valid_tile(former_x, tile_y)) {
			player_motion.position = {player_motion.formerPosition.x, player_motion.position.y};
		} else {
			player_motion.position = player_motion.formerPosition;
		}
	}
	return true;
}

void WorldSystem::player_respawn() {
	printf("Respawning player\n");

	Player& player = registry.get<Player>(player_entity);
	player.health = PLAYER_HEALTH;

	Motion& player_motion = registry.get<Motion>(player_entity);

// 	player_motion.position.x = WINDOW_WIDTH_PX / 2;
// 	player_motion.position.y = WINDOW_HEIGHT_PX / 2;
// 	player_motion.acceleration = { 0, 0 };
// 	player_motion.velocity = { 0, 0 };


	player_motion.position = vec2(spawnX, spawnY);
	player_motion.velocity = {0.f, 0.f};
	player_motion.acceleration = {0.f, 0.f};
	player_motion.formerPosition = vec2(spawnX, spawnY);
	UISystem::updatePlayerHealthBar(registry, PLAYER_HEALTH);
}


// Reset the world state to its initial state
void WorldSystem::restart_game() {

	std::cout << "Restarting..." << std::endl;

	// Debugging for memory/component leaks
	// registry.list_all_components();

	// Reset the game speed
	current_speed = 1.f;

	points = 0;
	next_invader_spawn = 0;
	invader_spawn_rate_ms = INVADER_SPAWN_RATE_MS;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	auto motions = registry.view<Motion>(entt::exclude<Player, Ship, Background>);
	registry.destroy(motions.begin(), motions.end());

	// createMob(registry, vec2(WINDOW_WIDTH_PX / 3, WINDOW_WIDTH_PX / 3));
	/*createRockType1(registry, vec2(WINDOW_WIDTH_PX / 4, WINDOW_WIDTH_PX / 4));
	createTreeType1(registry, vec2(WINDOW_WIDTH_PX / 4 + WINDOW_WIDTH_PX / 2, WINDOW_WIDTH_PX / 4 + WINDOW_WIDTH_PX / 4));*/
	// Reset player health
// 	auto player = registry.get<Player>(player_entity);
// 	player.health = PLAYER_HEALTH;
	
// 	// Reset player position
// 	auto motion = registry.get<Motion>(player_entity);
// 	motion.position = vec2(WINDOW_WIDTH_PX / 2, WINDOW_HEIGHT_PX / 2); 


	createMob(registry, vec2(spawnX - (50 * 16.f), spawnY));
	player_respawn();
	createPlayerHealthBar(registry, {spawnX, spawnY});
	createInventory(registry);

	// reset the screen
	auto screens = registry.view<ScreenState>();
	auto& screen = registry.get<ScreenState>(screens.front());
	screen.darken_screen_factor = 0;

}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// on key callback
void WorldSystem::on_key(int key, int, int action, int mod) {

	// exit game w/ ESC
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		close_window();
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
	// 		}
	// 	}
	// }
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {

	// record the current mouse position
	mouse_pos_x = mouse_position.x;
	mouse_pos_y = mouse_position.y;
}


void WorldSystem::on_mouse_button_pressed(int button, int action, int mods) {
	// on button press
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {

			printf("Mouse clicked at: (%f, %f)\n", mouse_pos_x, mouse_pos_y);

			auto& player_motion = registry.get<Motion>(player_entity);

			vec2 player_to_mouse_direction = vec2(mouse_pos_x, mouse_pos_y) - vec2(WINDOW_WIDTH_PX / 2, WINDOW_HEIGHT_PX / 2);

			vec2 direction = normalize(player_to_mouse_direction); // player position is always at (0, 0) in camera space

			vec2 velocity = direction * PROJECTILE_SPEED;

			if (!UISystem::useItemFromInventory(registry, mouse_pos_x, mouse_pos_y)) {
				createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity);
			}
			
		}
	
	}
}

