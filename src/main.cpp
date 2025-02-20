
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
using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	entt::registry reg;

	// global systems
	// AISystem	  ai_system;
	WorldSystem   world_system(reg);
	RenderSystem  renderer_system(reg);
	AISystem ai_system(reg);
	CollisionSystem collision_system(reg, world_system);
	PhysicsSystem physics_system(reg, collision_system);
	CameraSystem camera_system(reg, world_system);
	// PhysicsSystem physics_system;

	// initialize window
	GLFWwindow* window = world_system.create_window();
	if (!window) {
		// Time to read the error message
		std::cerr << "ERROR: Failed to create window.  Press any key to exit" << std::endl;
		getchar();
		return EXIT_FAILURE;
	}

	if (!world_system.start_and_load_sounds()) {
		std::cerr << "ERROR: Failed to start or load sounds." << std::endl;
	}

	// initialize the main systems
	renderer_system.init(window);
	world_system.init();

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

		num_s += elapsed_ms / 1000;
		num_frames++;

		// Display new frame rate every half-second
		if (num_s > 0.5) {
			std::stringstream title_ss;
			title_ss << "FPS: ";
			title_ss << std::fixed << std::setprecision(3) << (num_frames / num_s);
			glfwSetWindowTitle(window, title_ss.str().c_str());

			num_frames = 0;
			num_s = 0.f;
		}

		// Make sure collision_system is called before collision is after physics will mark impossible movements in a set
		world_system.step(elapsed_ms);
		physics_system.step(elapsed_ms);
		collision_system.step(elapsed_ms);
		camera_system.step(elapsed_ms);
		renderer_system.draw();
		ai_system.step(elapsed_ms); // AI system should be before physics system

	}

	return EXIT_SUCCESS;
}
