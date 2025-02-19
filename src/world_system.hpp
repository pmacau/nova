#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>
#include <entt.hpp>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"

enum KeyboardState {
	UP, DOWN, LEFT, RIGHT, NUM_STATES
};

// Container for all our entities and game logic.
// Individual rendering / updates are deferred to the update() methods.
class WorldSystem
{
public:

	WorldSystem(entt::registry& reg);

	// creates main window
	GLFWwindow* create_window();

	// starts and loads music and sound effects
	bool start_and_load_sounds();

	// call to close the window
	void close_window();

	// starts the game
	void init();

	// releases all associated resources
	~WorldSystem();

	// steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// should the game be over ?
	bool is_over() const;

	// player respawn
	void player_respawn();

private:
	entt::entity player_entity;
	entt::registry& registry;
	
	bool key_state[KeyboardState::NUM_STATES];
	float mouse_pos_x = 0.0f;
	float mouse_pos_y = 0.0f;

	// input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button_pressed(int button, int action, int mods);

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow* window;

	int next_invader_spawn;
	int invader_spawn_rate_ms;	// see default value in common.hpp

	int max_towers;	// see default value in common.hpp

	// Number of invaders stopped by the towers, displayed in the window title
	unsigned int points;

	// Game state
	float current_speed;

	// inventory
	std::vector<entt::entity> inventory;

	// grid
	std::vector<entt::entity> grid_lines;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* chicken_dead_sound;
	Mix_Chunk* chicken_eat_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
