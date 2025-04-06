
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
#include "collision/collision_system.hpp"
#include "physics_system.hpp"
#include "music_system.hpp"
#include "spawn_system.hpp"
#include "map/map_system.hpp"
#include "flag_system.hpp"
#include "map/generate.hpp"
#include "map/image_gen.hpp"
#include "animation_system.hpp"
#include "player/player_system.hpp"
#include <ai/ai_initializer.hpp>
#include <ai/state_machine/state_factory.hpp>
#include "quadtree/quadtree.hpp"

#include <iomanip>
using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// TOGGLE this if you don't want a new map every time...
	int mapWidth = 500, mapHeight = 500; 
	if (true) {
		auto generated_map = create_map(mapWidth, mapHeight);
		create_background(generated_map);
		create_biome_map(generated_map);
		create_terrain_map(generated_map);
		create_decoration_map(generated_map);

		save_map(generated_map, map_path("map.bin").c_str());
	}

	entt::registry reg;

	// assets and constants
	initializeAIStates(g_stateFactory);
	// QuadTree
	QuadTree quadTree((mapWidth / 2) * 16.f, (mapHeight / 2) * 16.f, mapWidth, mapHeight);
	// global systems
	FlagSystem flag_system(reg); 
	PhysicsSystem physics_system(reg, flag_system);
	WorldSystem   world_system(reg, physics_system, flag_system, quadTree);
	RenderSystem  renderer_system(reg, quadTree);
	AISystem ai_system(reg);
	CollisionSystem collision_system(reg, world_system, physics_system, quadTree);
	CameraSystem camera_system(reg);
	SpawnSystem spawn_system(reg);
	// FlagSystem flag_system(reg); 
	AnimationSystem animationSystem(reg);
	PlayerSystem playerSystem(reg);
	
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
	quadTree.initTree(reg); 
	//renderer_system.initTree(); 
	//collision_system.initTree(mapWidth, mapHeight);
	
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
			time_exe<int>("PHYS", [&](){physics_system.step(elapsed_ms); return 0;});
			time_exe<int>("WORL", [&](){world_system.step(elapsed_ms); return 0;});
			time_exe<int>("PLAY", [&](){playerSystem.update(elapsed_ms); return 0;});
			time_exe<int>("ANIM", [&](){animationSystem.update(elapsed_ms); return 0;});
			if (flag_system.isDone()) {
				time_exe<int>("SPAW", [&](){spawn_system.update(elapsed_ms); return 0;});	
			}
			time_exe<int>("COLL", [&](){collision_system.step(elapsed_ms); return 0;});
			time_exe<int>("CAME", [&](){camera_system.step(elapsed_ms); return 0;});
			time_exe<int>("AI  ", [&](){ai_system.step(elapsed_ms); return 0;}); // AI system should be before physics system
		}

		time_exe<int>("FLAG", [&](){flag_system.step(elapsed_ms); return 0;});
		time_exe<int>("REND", [&](){renderer_system.draw(); return 0;});
		debug_printf(DebugType::TIME, "-----------------------\n");
		set_debug(DebugType::TIME, false);
	}

	return EXIT_SUCCESS;
}
