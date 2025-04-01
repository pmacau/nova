#pragma once
#include "../common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include <cmath>
#include <limits>
#include <entt.hpp>
#include <animation/animation_definition.hpp>

struct Glyph{};
struct Tree{};
struct Background{};

struct Boss{
	float agro_range;
	vec2 spawn;
};

struct Slash {
	float damage = 1000.f; // change after
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
	enum class BulletType {
		GOLD_PROJ,
		BLASTER_PROJ,
		MISSLES_PROJ,
		RAILGUN_PROJ,
		SMG_PROJ
	};

	BulletType bulletType;
	int range;
	int health;
	float timer;
};

struct Dash {
	float cooldown = -1.f;
	float remainingDuration = 0.15f; 
	bool inUse = false; 
};

struct ShipWeapon
{
};

struct ShipEngine
{
};

struct UIShip
{

};

struct DebugTile
{
};

struct UIShipWeapon
{
	bool active;
};

struct UIShipEngine
{
	bool active;
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

struct HomingMissile {
	entt::entity target;
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

struct UIIcon
{

};

struct PlayerHealthBar
{
};

struct Button
{

};

struct UpgradeButton
{
	std::string text;
	bool missingResources = false;
	std::string missingResourcesText;
};

struct MobHealthBar
{
	entt::entity entity;
	int initial_health;
	float left_adjust = 0.f;
	float y_adjust = 0.f;
};

struct DeathItems {

};

struct Grave {

};

struct Item
{
	enum class Type {
		POTION,
		GRAVE, 
		IRON, 
		COPPER, 
		DEFAULT_WEAPON, 
		HOMING_MISSILE, 
		SHOTGUN
	};
	Type type;
	int no = 1;
};

// used for entities which when killed will drop items (usually bosses)
struct Drop
{
	std::vector<Item> items;
};

struct Potion
{
	int heal;
};

struct Title
{

};

struct TitleOption
{
	enum class Option {
		PLAY, 
		LOAD, 
		SAVE, 
		EXIT, 
		RESTART
	};
	Option type;
	std::string text;
	vec2 position;
	vec2 size;
	bool hover = false;
};

struct ButtonOption
{
	enum class Option {
		// for the upgrade screen
		SHIP,
		PLAYER,
		WEAPON,

		// for ship upgrade screen
		SHIP_HEALTH_UPGRADE,
		SHIP_BLASTER_UPGRADE,
		SHIP_RANGE_UPGRADE,
		SHIP_FIRERATE_UPGRADE,
	};
	Option type;
	std::string text;
	vec2 position;
	vec2 size;
	bool hover = false;
};

enum class Click {
	LEFT, 
	RIGHT, 
	CTRLRIGHT, 
	SHIFTRIGHT,
	ALTRIGHT
};

struct Drag {
	bool noSlot = false;
	entt::entity slot;
};

struct Inventory
{
	std::vector<entt::entity> slots;
};

struct HiddenInventory
{

};

struct ActiveSlot
{

};

struct InventorySlot
{
	int id = -1;
	bool hasItem = false;
	entt::entity item;
	int capacity = 50;
};

struct TextData
{
	std::string content;
	float scale;
	vec3 color;
	bool active = false;
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
	SHIP_FULL_HP,
	SHIP_SLIGHT_DAMAGE,
	SHIP_DAMAGE,
	SHIP_VERY_DAMAGE,
	SHIP_BLASTER_WEAPON,
	SHIP_MISSLES_WEAPON,
	SHIP_RAILGUN_WEAPON,
	SHIP_SMG_WEAPON,
	SHIP_BLASTER_ENGINE,
	SHIP_MISSLE_ENGINE,
	SHIP_RAILGUN_ENGINE,
	SHIP_SMG_ENGINE,
    MOB,
	TILESET,
	MAP_BACKGROUND,
	GOLD_PROJECTILE, 
	BLASTER_PROJECTILE,
	MISSILE_PROJECTILE,
	RAILGUN_PROJECTILE,
	SMG_PROJECTILE,
	SHOTGUN_PROJECTILE,
	DEFAULT_WEAPON, 
	HOMING_MISSILE, 
	SHOTGUN,
	HEALTHBAR_RED,
	PLAYER_HEALTH_INNER, 
	PLAYER_HEALTH_OUTER,
	POTION,
	GRAVE,
	IRON,
	COPPER,
	INVENTORY_SLOT,
	INVENTORY_SLOT_ACTIVE,
	TREE,
	GOBLIN_TORCH_BLUE,
	TITLE, 
	TEXTBOX_BACKGROUND,
	SELECTION_BUTTON,
	GREEN_BUTTON_ACTIVE,
	GREEN_BUTTON_PRESSED,
	RED_BUTTON_ACTIVE,
	RED_BUTTON_PRESSED,
	MINIMAP,
	TEXT,
	TEXTURE_COUNT
};


const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	TEXTURED, VIGNETTE, COLOURED, DEBUG, TEXT, LINE, E_SNOW, E_FOG, E_HEAT, E_RAIN, EFFECT_COUNT
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

// Sets the brightness of the screen
struct ScreenState
{
	enum class ScreenType {
        GAMEPLAY,
		UPGRADE_UI,
        SHIP_UPGRADE_UI,
		PLAYER_UPGRADE_UI,
		WEAPON_UPGRADE_UI,
		TITLE
    };

    ScreenType current_screen;
	EFFECT_ASSET_ID curr_effect = EFFECT_ASSET_ID::VIGNETTE;

	float time = 0;
	float darken_screen_factor = 0;
};