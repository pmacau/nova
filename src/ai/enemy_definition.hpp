#pragma once

#include "common.hpp"      // for vec2 and any common types
#include "ai_common.hpp"   // for CreatureType (if defined here) or define it here

#include <vector>
#include <string>

// Structure representing an animation sequence for a sprite.
struct SpriteAnimation {
    std::string name;           // Animation name (e.g., "idle", "attack")
    std::vector<vec2> frames;   // Coordinates in the spritesheet for each frame
    float frameDuration;        // Duration of each frame in milliseconds
};

// Group properties for spawn.
struct GroupProperties {
    int minSize; // Minimum number in a group.
    int maxSize; // Maximum number in a group.
};

// The enemy definition structure.
// This holds all data relevant for creating an enemy creature.
struct EnemyDefinition {
    CreatureType creatureType;  // Type: Mob, Boss, or Mutual
    std::string id;             // Unique identifier (e.g., "goblin", "skeleton")

    // Spawn-related parameters.
    float spawnProbability;      // Range 0.0 to 1.0.
    GroupProperties group;       // Group spawn settings.
    std::vector<int> biomeIDs;   // Biomes where this enemy can spawn.

    // Core gameplay stats.
    int minHealth;
    int maxHealth;
    int damage;
    float speed;

    // Rendering and animation properties.
    vec2 scale;                     // The default scale for rendering the sprite.
    std::string spriteSheetPath;    // Path to the spritesheet asset.
    std::vector<SpriteAnimation> animations; // List of animations (idle, run, attack, etc.)

    // Drop information.
    std::vector<std::string> dropItems; // Item IDs that can drop when defeated.

    // Future parameters (like AI-specific settings, agro range, etc.) can be added here.
};

extern std::vector<EnemyDefinition> enemyDefinitions;
void initializeEnemyDefinitions();