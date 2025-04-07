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

// Switch between 0 and 1 to toggle weather effects
#define WEATHER 0

inline std::string data_path() { return std::string(PROJECT_SOURCE_DIR) + "data"; };
inline std::string shader_path(const std::string& name) {return std::string(PROJECT_SOURCE_DIR) + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) {return data_path() + "/textures/" + std::string(name);};
//inline std::string textures_path(const std::string& name) {return data_path() + "/retextures/" + std::string(name);};
inline std::string audio_path(const std::string& name) {return data_path() + "/audio/" + std::string(name);};
inline std::string map_path(const std::string& name)  {return data_path() + "/maps/" + std::string(name);};

//
// game constants
//
const int WINDOW_WIDTH_PX = 960 * 1.5;
const int WINDOW_HEIGHT_PX = 540 * 1.5;

const int GRID_CELL_WIDTH_PX = 60;
const int GRID_CELL_HEIGHT_PX = 60;
const int GRID_LINE_WIDTH_PX = 2;

// Ship stats
const float SHIP_TIMER_S = 1.f;
const int SHIP_HEALTH = 500;
const int SHIP_RANGE = 250;

// Ship stats upgrades
const int SHIP_HEALTH_UPGRADE = 200;
const int SHIP_RANGE_UPGRADE = 100;
const int SHIP_DAMAGE_UPGRADE = 10;
const float SHIP_TIMER_UPGRADE = 0.15f;


// Max ship stats
const int SHIP_MAX_HEALTH = 1500;
const int SHIP_MAX_RANGE = 1000;

// projectile damages
const int PROJECTILE_DAMAGE = 20;
const int SMG_PROJ_DAMAGE = 25;
const int MISSLE_PROJ_DAMAGE = 30;
const int BLASTER_PROJ_DAMAGE = 40;
const int RAILGUN_PROJ_DAMAGE = 50;


// Max Weapon stast
const int PISTOL_MAX_DAMAGE = 34;
const float PISTOL_MAX_COOLDOWN = 0.2;

const int HOMING_MISSLE_MAX_DAMAGE = 69;
const float HOMING_MISSLE_MAX_COOLDOWN = 0.8;

const int SHOTGUN_MAX_DAMAGE = 34;
const float SHOTGUN_MAX_COOLDOWN = 0.4;

const float MELEE_MAX_DAMAGE = 20.0f;
const float MELEE_MAX_FORCE = 325.0f;


// player/mob stats
const int PLAYER_HEALTH = 100;
const float PLAYER_SPEED = 155.0f;

const int MOB_HEALTH = 50; 
const float MOB_SPEED = 135.0f; 
const int MOB_DAMAGE = 25; 


// player max stats
const int PLAYER_MAX_HEALTH = 200;
const float PLAYER_MAX_SPEED = 305.0f;
const float PLAYER_MAX_VISION_RADIUS = 0.7;


const float GAME_SCALE = 2.f;


// projectile stats
const float PROJECTILE_SPEED = 500.0f;
const float SMG_PROJ_SPEED = 600.0f;
const float MISSLE_PROJ_SPEED = 700.0f;
const float BLASTER_PROJ_SPEED = 800.0f;
const float RAILGUN_PROJ_SPEED = 1000.0f;

const float PROJECTILE_SIZE = 20.0f;
const int PROJECTILE_TIMER = 5000;


const int MAX_INVENTORY_SLOTS = 5;


// melee stats
const float MELEE_DAMAGE = 5.0f;
const float MELEE_FORCE = 250.0f;
const float MELEE_COOLDOWN = 0.80;

const float WEAPON_COOLDOWN = 0.5;
const float MOUSE_POLL_RATE = 100;


// Player Upgrade Constants
const int PLAYER_HEALTH_UPGRADE_IRON = 5; //5
const int PLAYER_HEALTH_UPGRADE_COPPER = 3; //3

const int PLAYER_VISION_UPGRADE_COPPER = 6; //6

const int PLAYER_SPEED_UPGRADE_IRON = 3; //3
const int PLAYER_SPEED_UPGRADE_COPPER = 6; //6


// Ship Upgrade Constants
const int SHIP_HEALTH_UPGRADE_IRON = 7; // 7

const int SHIP_WEAPON_UPGRADE_IRON = 3; // 3
const int SHIP_WEAPON_UPGRADE_COPPER = 3; // 3

const int SHIP_FIRERATE_UPGRADE_IRON = 2; // 2
const int SHIP_FIRERATE_UPGRADE_COPPER = 4; // 4

const int SHIP_RANGE_UPGRADE_IRON = 5; // 5


// Weapon Upgrade Constants
const int PISTOL_UPGRADE_IRON = 5;

const int HOMING_MISSLE_UPGRADE_IRON = 7;
const int HOMING_MISSLE_UPGRADE_COPPER = 2;

const int SHOTGUN_UPGRADE_IRON = 6;
const int SHOTGUN_UPGRADE_COPPER = 3;

const int MELEE_UPGRADE_IRON = 5; //5


// Weapon Unlock Constants
const int HOMING_MISSLE_UNLOCK_IRON = 8;
const int HOMING_MISSLE_UNLOCK_COPPER = 4;

const int SHOTGUN_UNLOCK_IRON = 10;
const int SHOTGUN_UNLOCK_COPPER = 5;


// Weather
const bool ENABLE_WEATHER = true;

// Camera system
const float CAMERA_SPEED = 0.2f;
const float CAMERA_ZOOM_SPEED = 0.1f;
const float CAMERA_PLAYER_DIST = 100.0f;
const float CAMERA_ANGLE = M_PI / 4.0f;


// Spawn system
const vec2 SPAWN_SAFE_ZONE = vec2(1060, 640);   // a bit bigger than screen
const vec2 SPAWN_ZONE = vec2(1260, 840);
const vec2 DESPAWN_ZONE = vec2(1560, 1140);

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

