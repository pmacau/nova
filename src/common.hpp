#pragma once

// stlib
#include <fstream> // stdout, stderr..
#include <string>
#include <tuple>
#include <vector>
#include <numbers>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
using namespace glm;

// Simple utility functions to avoid mistyping directory name
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "../ext/project_path.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif


inline std::string data_path() { return std::string(PROJECT_SOURCE_DIR) + "data"; };
inline std::string shader_path(const std::string& name) {return std::string(PROJECT_SOURCE_DIR) + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) {return data_path() + "/textures/" + std::string(name);};
//inline std::string textures_path(const std::string& name) {return data_path() + "/retextures/" + std::string(name);};
inline std::string audio_path(const std::string& name) {return data_path() + "/audio/" + std::string(name);};
inline std::string map_path(const std::string& name)  {return data_path() + "/maps/" + std::string(name);};

//
// game constants
//
// const int WINDOW_WIDTH_PX = 840;
// const int WINDOW_HEIGHT_PX = 600;
const int WINDOW_WIDTH_PX = 960;
const int WINDOW_HEIGHT_PX = 540;

const int GRID_CELL_WIDTH_PX = 60;
const int GRID_CELL_HEIGHT_PX = 60;
const int GRID_LINE_WIDTH_PX = 2;

const int SHIP_TIMER_MS = 500;
const int SHIP_HEALTH = 500;
const int SHIP_RANGE = 250;

const int PROJECTILE_DAMAGE = 20;

const int PLAYER_HEALTH = 100;
const int MOB_HEALTH = 50; 
const float PLAYER_SPEED = 200.0f;
const float MOB_SPEED = 100.0f; 

const float GAME_SCALE = 2.f;

const int MOB_DAMAGE = 25; 
const float PROJECTILE_SPEED = 500.0f;
const float PROJECTILE_SIZE = 20.0f;
const int PROJECTILE_TIMER = 5000;

const int MAX_INVENTORY_SLOTS = 5;

const float WEAPON_COOLDOWN = 0.5;
const float MOUSE_POLL_RATE = 100;


// Camera system
const float CAMERA_SPEED = 0.2f;
const float CAMERA_ZOOM_SPEED = 0.1f;
const float CAMERA_PLAYER_DIST = 100.0f;
const float CAMERA_ANGLE = M_PI / 4.0f;


// Spawn system
const vec2 SPAWN_SAFE_ZONE = vec2(1000, 600);   // a bit bigger than screen
const vec2 SPAWN_ZONE = vec2(1200, 800);
const vec2 DESPAWN_ZONE = vec2(1500, 1100);

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
// We recommend making all components non-copyable by derving from ComponentNonCopyable
struct Transform {
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

bool gl_has_errors();

