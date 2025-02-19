// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "tinyECS/components.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>

// create the world
WorldSystem::WorldSystem(entt::registry& reg) :
	registry(reg),
	next_invader_spawn(0),
	invader_spawn_rate_ms(INVADER_SPAWN_RATE_MS),
	max_towers(MAX_TOWERS_START),
	points(0)
{
	// seeding rng with random device
	player_entity = createPlayer(registry, vec2(WINDOW_WIDTH_PX / 2, WINDOW_WIDTH_PX / 2));
	for (auto i = 0; i < KeyboardState::NUM_STATES; i++) key_state[i] = false;

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
	
	// Updating window title with points
	std::stringstream title_ss;
	float s_elapsed = elapsed_ms_since_last_update / 1000;

	title_ss << std::fixed << std::setprecision(3) << "FPS: " << (1 / s_elapsed);
	glfwSetWindowTitle(window, title_ss.str().c_str());

	auto player = registry.get<Player>(player_entity);
	if (player.health <= 0) {
		printf("[GAME OVER] restarting game now...\n");
		restart_game();
	}
	
	// TODO: move player direction system
	auto& p_motion = registry.get<Motion>(player_entity);
	auto& p_sprite = registry.get<Sprite>(player_entity);

	switch (player.direction) {
		case KeyboardState::UP:
			p_sprite.coord.x = 2.f;
			p_motion.scale.x = abs(p_motion.scale.x);
			break;
		case KeyboardState::DOWN:
			p_sprite.coord.x = 0.f;
			p_motion.scale.x = abs(p_motion.scale.x);
			break;
		case KeyboardState::LEFT:
			p_sprite.coord.x = 1.f;
			p_motion.scale.x = -1.f * abs(p_motion.scale.x);
			break;
		case KeyboardState::RIGHT:
			p_sprite.coord.x = 1.f;
			p_motion.scale.x = abs(p_motion.scale.x);
			break;
	}

	// TODO: refactor this logic to be more reusable/modular i.e. make a helper to update player speed based on key state
	auto updatePlayerVelocity = [this]() {
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
	updatePlayerVelocity();

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

	return true;
}

void WorldSystem::player_respawn() {
	// reset player health
	Player& player = registry.get<Player>(player_entity);
	player.health = PLAYER_HEALTH;

	// reset player position
	// Motion& player_motion = registry.get<Motion>(player_entity);
	// player_motion.position.x = WINDOW_WIDTH_PX / 2;
	// player_motion.position.y = WINDOW_HEIGHT_PX / 2;
}


// Reset the world state to its initial state
void WorldSystem::restart_game() {

	std::cout << "Restarting..." << std::endl;

	// Debugging for memory/component leaks
	// registry.list_all_components();

	// Reset the game speed
	current_speed = 1.f;

	points = 0;
	max_towers = MAX_TOWERS_START;
	next_invader_spawn = 0;
	invader_spawn_rate_ms = INVADER_SPAWN_RATE_MS;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	auto motions = registry.view<Motion>(entt::exclude<Player>);
	registry.destroy(motions.begin(), motions.end());
	createMob(registry, vec2(WINDOW_WIDTH_PX / 2, WINDOW_WIDTH_PX / 2));
	
	// Reset player health
	// auto& player = registry.get<Player>(player_entity);
	// player.health = PLAYER_HEALTH;

	// // Reset player position
	// auto& motion = registry.get<Motion>(player_entity);
	// motion.position = vec2(start_col * 16, start_row * 16);
	player_respawn();
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

	if (action != GLFW_PRESS) return;

	auto& player = registry.get<Player>(player_entity);
	if (key == GLFW_KEY_UP    || key == GLFW_KEY_W) player.direction = KeyboardState::UP;
	if (key == GLFW_KEY_DOWN  || key == GLFW_KEY_S) player.direction = KeyboardState::DOWN;
	if (key == GLFW_KEY_LEFT  || key == GLFW_KEY_A) player.direction = KeyboardState::LEFT;
	if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) player.direction = KeyboardState::RIGHT;

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
		std::cout << "mouse position: " << mouse_pos_x << ", " << mouse_pos_y << std::endl;
	
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			// TODO: implement shooting logic

			auto& player_motion = registry.get<Motion>(player_entity);

			vec2 direction = normalize(vec2(mouse_pos_x, mouse_pos_y) - player_motion.position);

			vec2 velocity = direction * PROJECTILE_SPEED;

			createProjectile(registry, player_motion.position, vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), velocity);
			
		}
	
	}
}

