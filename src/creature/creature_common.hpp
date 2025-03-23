#pragma once
#include <glm/glm.hpp>
#include <tinyECS/components.hpp>
#include <map/tile.hpp>


enum class CreatureType {
    Mob,
    Boss,
    Mutual,
};

struct SpriteAnimation {
    std::string name;           // Animation name (e.g., "idle", "attack")
    std::vector<vec2> frames;   // Coordinates in the spritesheet for each frame
    float frameDuration;        // Duration of each frame in milliseconds
};

struct GroupProperties {
    int minSize; // Minimum number in a group.
    int maxSize; // Maximum number in a group.
};

struct CreatureDefinition {
    CreatureType creatureType;  // Type: Mob, Boss, or Mutual
    std::string id;             // Unique identifier (e.g., "goblin", "skeleton")

    // Spawn-related parameters.
    float spawnProbability;      // Range 0.0 to 1.0.
    GroupProperties group;       // Group spawn settings.
    std::vector<Biome> biomes;   // Biomes where this enemy can spawn.

    // Core gameplay stats.
    int minHealth;
    int maxHealth;
    int damage;
    float speed;

    // Rendering and animation properties.
    vec2 scale;                     // The default scale for rendering the sprite.
    TEXTURE_ASSET_ID textureAssetID;    // Path to the spritesheet asset.
    std::vector<SpriteAnimation> animations; // List of animations (idle, run, attack, etc.)

    // Drop information.
    std::vector<std::string> dropItems; // Item IDs that can drop when defeated.

    // AI-specific settings, agro range
};