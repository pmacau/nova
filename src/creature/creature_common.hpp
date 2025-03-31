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
    BLUE_TORCH_GOBLIN,
    SKELETON,
    BOSS,
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

struct DropInfo {
    std::vector<std::string> dropItems;
};

struct PhysicsInfo {
    vec2 scale; // used for motion
    Hitbox hitbox;
    vec2 offset_to_ground = {0 ,0};
};

struct AIInfo {
    AIConfig aiConfig;
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
