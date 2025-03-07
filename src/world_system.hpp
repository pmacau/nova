#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>
#include <entt.hpp>

#include "render_system.hpp"
#include "physics_system.hpp"

enum KeyboardState {
	UP, DOWN, LEFT, RIGHT, SHIPUI, NUM_STATES,
};

class WorldSystem
{
public:

	WorldSystem(entt::registry& reg, PhysicsSystem& physics_system);

	// creates main window
	GLFWwindow* create_window();

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
	entt::registry& registry;
	entt::entity player_entity;
	PhysicsSystem& physics_system;
	entt::entity ship_entity;
	entt::entity main_camera_entity;
	entt::entity screen_entity;

	vec2 player_spawn = {0.f, 0.f};
	
	bool key_state[KeyboardState::NUM_STATES];
	float mouse_pos_x = 0.0f;
	float mouse_pos_y = 0.0f;

	float mouse_click_poll = MOUSE_POLL_RATE;

	// input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button_pressed(int button, int action, int mods);
	void left_mouse_click();

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow* window;


	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
