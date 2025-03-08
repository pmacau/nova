
#define GL3W_IMPLEMENTATION
#include <gl3w.h>
#include <entt.hpp>

// stdlib
#include <chrono>
#include <iostream>

// internal
#include "render_system.hpp"
#include "world_system.hpp"
#include "camera_system.hpp"
#include "ai_system.hpp"
#include "collision_system.hpp"
#include "physics_system.hpp"
#include "music_system.hpp"
#include "spawn_system.hpp"
#include "map/map_system.hpp"
#include "flag_system.hpp"
#include "map/generate.hpp"
#include "map/image_gen.hpp"

#include <iomanip>
using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// TOGGLE this if you don't want a new map every time...
	if (true) {
		auto generated_map = create_map(200, 200);
		create_background(generated_map);
		save_map(generated_map, map_path("map.bin").c_str());
	}

	entt::registry reg;

	// assets and constants
	initializeSpawnDefinitions();

	// global systems
	PhysicsSystem physics_system(reg);
	FlagSystem flag_system(reg); 
	WorldSystem   world_system(reg, physics_system, flag_system);
	RenderSystem  renderer_system(reg);
	AISystem ai_system(reg);
	CollisionSystem collision_system(reg, world_system, physics_system);
	CameraSystem camera_system(reg, world_system);
	SpawnSystem spawn_system(reg);
	

	// initialize window
	GLFWwindow* window = world_system.create_window();
	if (!window) {
		// Time to read the error message
		std::cerr << "ERROR: Failed to create window.  Press any key to exit" << std::endl;
		getchar();
		return EXIT_FAILURE;
	}

	if (!MusicSystem::init()) {
		std::cerr << "ERROR: Failed to start or load sounds." << std::endl;
	}

	// initialize the main systems
	MapSystem::init(reg);
	world_system.init();
	renderer_system.init(window);
	renderer_system.initFreetype();

	// variable timestep loop
	auto t = Clock::now();

	int num_frames = 0;
	float num_s = 0.f;

	while (!world_system.is_over()) {
		
		// processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// calculate elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;
		// frame count for collision checks

		num_s += elapsed_ms / 1000;
		num_frames++;

		// Display new frame rate every half-second
		if (num_s > 0.5) {
			std::stringstream title_ss;
			title_ss << "Nova (FPS: ";
			title_ss << std::fixed << std::setprecision(3) << (num_frames / num_s);
			title_ss << ")";
			glfwSetWindowTitle(window, title_ss.str().c_str());

			num_frames = 0;
			num_s = 0.f;
		}


		// Make sure collision_system is called before collision is after physics will mark impossible movements in a set
		if (!flag_system.is_paused) {
			physics_system.step(elapsed_ms);
			world_system.step(elapsed_ms);
			spawn_system.update(elapsed_ms);
			collision_system.step(elapsed_ms);
			camera_system.step(elapsed_ms);
			ai_system.step(elapsed_ms); // AI system should be before physics system
		}
		
		flag_system.step(elapsed_ms);
		renderer_system.draw();
		
		
	}

	return EXIT_SUCCESS;
}
