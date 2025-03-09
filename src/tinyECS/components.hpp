#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include <cmath>
#include <limits>
#include <entt.hpp>
#include <animation/animation_definition.hpp>

struct Tree{};
struct Background{};

struct Boss{
	float agro_range;
	vec2 spawn;
};

struct InputState {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
};

struct Obstacle {
	bool isPassable; 
	bool isSlow; 
	bool isDamage;
	float slowFactor; 
	float damage;
};


// Player component
struct Player
{
	int health;
	float weapon_cooldown = WEAPON_COOLDOWN; // half a second weapon cooldown
};

// Ship component
struct Ship
{
	int range;
	int health;
	int timer;
};

struct UIShip
{

};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2  position = { 0, 0 };
	float angle    = 0;
	vec2  velocity = { 0, 0 };
	vec2  scale    = { 10, 10 };
	float zValue   = 0.f;

    vec2 offset_to_ground = { 0, 0 };  // Offset from top-left to ground position

	vec2 acceleration = { 0, 0 }; 
	vec2 formerPosition = {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()}; // used for map obstacles
};

// Invader
struct Invader {
	int health;
};

struct MarkedCollision {
	glm::vec2 velocity;
};

// Projectile
struct Projectile {
	int damage;
	int timer;
};


// Mob
struct Mob {
	int health;
	float hit_time; 
};

// used for Entities that cause damage
struct Deadly
{

};

// used for edible entities
struct Eatable
{

};

struct FixedUI
{

};

struct UI
{

};

struct PlayerHealthBar
{
};

struct MobHealthBar
{
	entt::entity entity;
	int initial_health;
	float left_adjust = 0.f;
};

enum class ITEM_TYPE {
	POTION
};

// used for entities which when killed will drop items (usually bosses)
struct Drop
{
	ITEM_TYPE item_type;
};

struct Item
{
	ITEM_TYPE item_type;
};

struct Potion
{
	int heal;
};

struct InventorySlot
{
	bool hasItem = false;
	entt::entity item;
};

struct Inventory
{
	std::vector<entt::entity> slots;
};

// Stucture to store collision information
// struct Collision
// {
// 	// Note, the first object is stored in the ECS container.entities
// 	Entity other; // the second object involved in the collision
// 	Collision(Entity& other) { this->other = other; };
// };

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	enum class ScreenType {
        GAMEPLAY,
        SHIP_UPGRADE_UI,
    };

    ScreenType current_screen;
	float darken_screen_factor = 0;
};

// will be given to any map object entity, then can also be given a rectangular or circular hitbox, different collision mechanism. 
struct Object {

};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	PLAYER,
	SHIP1,
	SHIP2,
	SHIP3,
	SHIP4,
	SHIP5,
	SHIP6,
    MOB,
	TILESET,
	MAP_BACKGROUND,
	GOLD_PROJECTILE, 
	HEALTHBAR_GREEN,
	HEALTHBAR_RED,
	POTION,
	INVENTORY_SLOT,
	TREE,
	GOBLIN_TORCH_BLUE,
	TEXTURE_COUNT
};


const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	TEXTURED, VIGNETTE, COLOURED, DEBUG, TEXT, EFFECT_COUNT
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPRITE, 
	SCREEN_TRIANGLE,
	DEBUG_POINT,
	GEOMETRY_COUNT
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest
{
	TEXTURE_ASSET_ID   used_texture  = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID    used_effect   = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

struct Sprite 
{
	FrameIndex coord = {0, 0};
	vec2 dims;
	vec2 sheet_dims;

	int up_row = 0;
	int down_row = 0;
	int right_row = 0;
};

struct Animation
{
    float frameDuration;       // Duration of each frame in milliseconds.
    float frameTime = 0.0f;      // Time accumulator.
    int totalFrames;           // Total number of frames in the current animation.
    int currentFrameIndex;     // Current frame index.
    int row;                   // The row in the spritesheet for this animation.
	float frameWidth;          // Width of a single frame (set during creation)
    float frameHeight;         // Height of a single frame (set during creation)
};

// Camera
struct Camera
{
	entt::entity target; // the entity the camera follows
	float distance_from_target = CAMERA_PLAYER_DIST; // position relative to (center of the screen)
	float angle = CAMERA_ANGLE; // angle relative to the player
	vec2 offset = vec2(0.f, 0.f); // offset from the player

	vec3 position = {0.f, 0.f, 0.f}; // inferenced 3D position for the camera
};

const Sprite PLAYER_SPRITESHEET = {
    {}, {19.f, 30.f}, {152.f, 90.f}, 3, 0, 1
};