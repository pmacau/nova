#include <creature/creature_common.hpp>
#include <creature/creature_manager.hpp>
#include <ai/ai_common.hpp>
#include "ai/state_machine/transition.hpp"

const TransitionTable& getGoblinTransitionTable() {
    static TransitionTable goblinTransitions;
    if (goblinTransitions.empty()) {
        // Transition from "patrol" to "chase"
        goblinTransitions["patrol"].push_back({
            "chase",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                return (length(diff) < config.detectionRange);
            }
        });
        // Transition from "chase" to "attack"
        goblinTransitions["chase"].push_back({
            "attack",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                return (length(diff) < config.attackRange);
            }
        });
        // Transition from "chase" back to "idle"
        goblinTransitions["chase"].push_back({
            "patrol",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                return (length(diff) > config.unchaseRange);
            }
        });
        // Transition from "attack" to "chase"
        goblinTransitions["attack"].push_back({
            "chase",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                float dist = length(diff);
                return (dist >= config.attackRange && dist < config.unchaseRange);
            }
        });

        // Transition from "attack" to "patrol"
        goblinTransitions["attack"].push_back({
            "patrol",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                return (length(diff) > config.unchaseRange);
            }
        });

    }
    return goblinTransitions;
}

AIConfig getGoblinAIConfig() {
    AIConfig config;

    config.detectionRange = 300.0f;
    config.unchaseRange = 500.0f;
    config.chaseSpeed = 100.0f;
    config.attackRange = 50.0f;
    config.patrolRadius = 100.0f;
    config.patrolSpeed = 25.f;
    config.patrolThreshold = 5.0f;
    return config;
}

void initializeDemoMobDefinition() {
    CreatureDefinition def;
    
    def.id = "goblin";
    def.creatureType = CreatureType::Mob;
    
    // Spawn Info.
    def.spawnInfo.spawnProbability = 0.5f;
    def.spawnInfo.group.minSize = 1;
    def.spawnInfo.group.maxSize = 2;
    def.spawnInfo.biomes = {Biome::B_FOREST, Biome::B_BEACH, Biome::B_ICE, Biome::B_JUNGLE, Biome::B_SAVANNA};
    
    // Gameplay Stats.
    def.stats.minHealth = 50;
    def.stats.maxHealth = 70;
    def.stats.damage = 10;
    def.stats.speed = 1.0f;
    
    // Rendering Info.
    def.renderingInfo.scale = vec2(1344.f / 7, 960.f / 5) * 0.9f;
    def.renderingInfo.spriteSheet.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;
    def.renderingInfo.spriteSheet.sheetDimensions = {1344.f, 960.f};
    def.renderingInfo.animationMapping = {
        {"idle", "mob2_idle"},
        {"walk", "mob2_walk"},
    };
    
    // physics
    def.physicsInfo.offset_to_ground = {0, def.renderingInfo.scale.y / 4.f * 0.8f};
    float w = def.renderingInfo.scale.x * 0.4;
    float h = def.renderingInfo.scale.y * 0.5;
    def.physicsInfo.hitbox.pts = {
        {w * -0.5f, h * -0.5f}, {w * 0.5f, h * -0.5f},
        {w * 0.5f, h * 0.5f},   {w * -0.5f, h * 0.5f}
    };

    def.physicsInfo.hitbox.depth = 60;

        
    // Drop Info.
    def.dropInfo.dropItems = {"potion"};
    
    // AI Info.
    def.aiInfo.aiConfig = getGoblinAIConfig();
    // Set the pointer to the shared transition table for goblins.
    def.aiInfo.transitionTable = &getGoblinTransitionTable();
    def.aiInfo.initialState = "patrol";
    
    CreatureManager::getInstance().addDefinition(def);
}


std::unordered_map<std::string, AnimationDefinition> initializeGoblinAnimation() {
    std::unordered_map<std::string, AnimationDefinition> animations;

    // Mob
    SpriteSheet mob_spritesheet;
    mob_spritesheet.textureAssetID = TEXTURE_ASSET_ID::GOBLIN_TORCH_BLUE;
    mob_spritesheet.sheetDimensions = {1344.f, 960.f};

    AnimationDefinition mob2Idle;
    mob2Idle.id = "mob2_idle";
    mob2Idle.loop = true;
    mob2Idle.frameWidth = 1344.f / 7;
    mob2Idle.frameHeight = 960.f / 5;
    mob2Idle.spriteSheet = mob_spritesheet;
    for (int col = 0; col < 7; ++col) {
        mob2Idle.frames.push_back({0, col});
        mob2Idle.frameDurations.push_back(150.f); // 150 ms per frame.
    }
    animations[mob2Idle.id] = mob2Idle;

    // --- Mob1 Walk Animation ---
    AnimationDefinition mob2Walk;
    mob2Walk.id = "mob2_walk";
    mob2Walk.loop = true;
    mob2Walk.frameWidth = 1344.f / 7;
    mob2Walk.frameHeight = 960.f / 5;
    mob2Walk.spriteSheet = mob_spritesheet;
    for (int col = 0; col < 8; ++col) {
        mob2Walk.frames.push_back({1, col});
        mob2Walk.frameDurations.push_back(100.f); // 100 ms per frame.
    }
    animations[mob2Walk.id] = mob2Walk;

    return animations;
}