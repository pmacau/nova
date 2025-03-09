#include "enemy_definition.hpp"
#include <glm/glm.hpp>
#include <tinyECS/components.hpp>

std::vector<EnemyDefinition> enemyDefinitions;

void initializeEnemyDefinitions() {
    enemyDefinitions.clear();

    {
        // testing
        EnemyDefinition def;
        def.id = "demo_mob";
        def.creatureType = CreatureType::Mob;
        def.spawnProbability = 0.5f;
        def.group.minSize = 1;
        def.group.maxSize = 2;
        def.biomes = {Biome::B_FOREST, Biome::B_BEACH};

        // Basic stats
        def.minHealth = 50;
        def.maxHealth = 70;
        def.damage = 10;
        def.speed = 1.0f;

        def.scale = glm::vec2(100, 120);
        def.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;

        // Animations: Suppose each frame is 64x64, row 0 = idle, row 1 = attack, etc.
        int frameWidth  = 64;
        int frameHeight = 64;

        // Idle animation (row 0 with 6 frames)
        {
            SpriteAnimation idle;
            idle.name = "idle";
            idle.frameDuration = 100.0f; // 100ms per frame
            int row = 0;
            int idleFrames = 6;
            for (int i = 0; i < idleFrames; i++) {
                // (x, y) of top-left corner of each frame
                glm::vec2 framePos = glm::vec2(i * frameWidth, row * frameHeight);
                idle.frames.push_back(framePos);
            }
            def.animations.push_back(idle);
        }

        // Attack animation (row 1 with 5 frames)
        {
            SpriteAnimation attack;
            attack.name = "attack";
            attack.frameDuration = 80.0f;
            int row = 1;
            int attackFrames = 5;
            for (int i = 0; i < attackFrames; i++) {
                glm::vec2 framePos = glm::vec2(i * frameWidth, row * frameHeight);
                attack.frames.push_back(framePos);
            }
            def.animations.push_back(attack);
        }

        // You can define more animations (run, death, etc.) similarly.

        // Items dropped on defeat
        def.dropItems = {"potion"};

        // Finally, push this definition into the global list
        enemyDefinitions.push_back(def);
    }

    // ... define other enemies similarly ...
}