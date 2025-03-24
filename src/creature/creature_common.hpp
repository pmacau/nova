#pragma once
#include <glm/glm.hpp>
#include <tinyECS/components.hpp>
#include <map/tile.hpp>
#include <animation/animation_definition.hpp>


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
    std::string startAnimationId;  // Initial animation ID.
};

struct DropInfo {
    std::vector<std::string> dropItems;
};

struct CreatureDefinition {
    CreatureType creatureType;  // Type: Mob, Boss, or Mutual
    std::string id;             // Unique identifier (e.g., "goblin", "skeleton")

    SpawnInfo spawnInfo;
    Stats stats;                
    RenderingInfo renderingInfo;
    DropInfo dropInfo;

    vec2 offset_to_ground = {0 ,0};

    // AI-specific settings, agro range
};