#pragma once
#include <glm/glm.hpp>
#include <tinyECS/components.hpp>
#include <map/tile.hpp>
#include <collision/hitbox.hpp>
#include <ai/ai_common.hpp>
#include <ai/state_machine/transition.hpp>

#include <animation/animation_definition.hpp>

enum class CreatureID {
    BASE_CREATURE,
    GOBLIN_TORCH_BLUE,
    GOBLIN_TORCH_RED,
	GOBLIN_TORCH_PURPLE,
	GOBLIN_TORCH_YELLOW,
    SMALL_BLUE_ARCHER,
    SMALL_PURPLE_ARCHER,
    SMALL_RED_ARCHER,
    SMALL_YELLOW_ARCHER,
    SKELETON,
    BOSS,
    BOSS_FOREST_PURPLE,
    BOSS_BEACH_RED,
    BOSS_JUNGLE_YELLOW,
};


enum class CreatureType {
    Mob,
    Boss,
    Mutual,
};

struct GroupProperties {
    int minSize; // Minimum number in a group.
    int maxSize; // Maximum number in a group.
};

struct SpawnInfo {
    float spawnProbability;         // 0.0 to 1.0.
    GroupProperties group;
    std::vector<Biome> biomes;      
};

// Core gameplay stats.
struct Stats {
    int minHealth;
    int maxHealth;
    int damage;
    float speed;
};

struct RenderingInfo {
    glm::vec2 scale;                      
    SpriteSheet spriteSheet;
    // Mapping of action names (e.g., "idle", "walk", "attack") to animation IDs.
    std::unordered_map<std::string, std::string> animationMapping;
    std::string initialAnimationId;  // Initial animation ID.

    MotionAction initAction;
    MotionDirection initDirection;
};

struct QuantityRange {
    int min;
    int max;
};

struct DropItem {
    Item::Type type;
    QuantityRange quantityRange;
    float probability; // Probability of dropping this item.
};

struct DropInfo {
    std::vector<DropItem> dropItems;
};

struct PhysicsInfo {
    vec2 scale; // used for motion
    Hitbox hitbox;
    vec2 offset_to_ground = {0 ,0};
};

struct AIInfo {
    std::shared_ptr<AIConfig> aiConfig;
    const TransitionTable* transitionTable = nullptr;
    std::string initialState;
};

struct CreatureDefinition {
    CreatureType creatureType;
    CreatureID id;             

    SpawnInfo spawnInfo;
    Stats stats;                
    RenderingInfo renderingInfo;
    PhysicsInfo physicsInfo;
    DropInfo dropInfo;
    AIInfo aiInfo;
};

struct UIInfo
{
    float healthBar_y_adjust = 0.f;
};
