#include "chase_state.hpp"
#include "ai/ai_component.hpp"
#include "ai/state_machine/attack_state.hpp"
#include "ai/ai_common.hpp"  // for CHASE_SPEED, ATTACK_RANGE, etc.
#include "tinyECS/components.hpp"
#include <cmath>
#include <iostream>
#include "patrol_state.hpp"
#include <ai/path_finder.hpp>
#include <map/map_system.hpp>

#include <util/debug.hpp>
#include <world_init.hpp>

static Pathfinder g_pathFinder;

void ChaseState::regeneratePath(entt::registry& registry, ivec2 startTile, ivec2 targetTile) {
    currentPath = g_pathFinder.findPath(startTile, targetTile);
    // remove the first tile since it is the current tile
    if (!currentPath.empty()) {
        currentPath.erase(currentPath.begin());
    }

    currentWaypointIndex = 0;
    pathRecalcTimer = 0.0f;

    if (debugMode) {
        for (auto tile : currentPath) {
            createDebugTile(registry, tile);
        }
    }
}


void ChaseState::onEnter(entt::registry& registry, entt::entity entity) {
    debug_printf(DebugType::AI, "ChaseState: onEnter, finding path to player\n");
    // Reset path-related variables.
    currentPath.clear();
    currentWaypointIndex = 0;
    pathRecalcTimer = 0.0f;
    
    // Compute the initial path.
    auto& motion = registry.get<Motion>(entity);
    vec2 footPos = motion.position + motion.offset_to_ground;
    
    // Retrieve player position.
    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0) return;

    auto playerEntity = *playerView.begin();
    auto& playerMotion = registry.get<Motion>(playerEntity);
    vec2 playerFootPos = playerMotion.position + playerMotion.offset_to_ground;
    
    // Convert enemy and player world positions to tile indices.
    ivec2 enemyTile = ivec2(MapSystem::get_tile_indices(footPos));
    ivec2 playerTile = ivec2(MapSystem::get_tile_indices(playerFootPos));

    regeneratePath(registry, enemyTile, playerTile);
}

void ChaseState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.75f, 1.5f);

    auto& motion = registry.get<Motion>(entity);
    vec2 footPos = motion.position + motion.offset_to_ground;
    
    // Update the path recalculation timer.
    pathRecalcTimer += deltaTime;
    const float pathRecalcInterval = 1000.0f; // Recalculate path every 1000ms.
    if (pathRecalcTimer >= pathRecalcInterval) {
        pathRecalcTimer = 0.0f;
        
        // Retrieve player's current position.
        auto playerView = registry.view<Player, Motion>();
        if (!(playerView.size_hint() == 0)) {
            auto playerEntity = *playerView.begin();
            auto& playerMotion = registry.get<Motion>(playerEntity);
            vec2 playerFootPos = playerMotion.position + playerMotion.offset_to_ground;

            ivec2 enemyTile = ivec2(MapSystem::get_tile_indices(footPos));
            ivec2 playerTile = ivec2(MapSystem::get_tile_indices(playerFootPos));

            regeneratePath(registry, enemyTile, playerTile);
        }
    }
    
    // Follow the path if available.
    if (!currentPath.empty() && currentWaypointIndex < currentPath.size()) {
        // The first node should be the enemy's current tile.
        // Use the next waypoint as target.
        ivec2 nextTile = currentPath[currentWaypointIndex];
        // Convert tile index to world coordinates (center of tile).
        vec2 nextWaypoint = MapSystem::get_tile_center_pos(vec2(nextTile.x, nextTile.y));
        
        vec2 toWaypoint = nextWaypoint - footPos;
        float distance = length(toWaypoint);
        const float waypointThreshold = TILE_SIZE / 2.f; // Threshold in world units.
        
        if (distance < waypointThreshold) {
            // Waypoint reached; move to next.
            currentWaypointIndex++;
            // If we've reached the final waypoint, check if we need to recalc.
            if (currentWaypointIndex >= currentPath.size()) {
                // Retrieve player's current position.
                auto playerView = registry.view<Player, Motion>();
                if (!(playerView.size_hint() == 0)) {
                    auto playerEntity = *playerView.begin();
                    auto& playerMotion = registry.get<Motion>(playerEntity);
                    vec2 playerFootPos = playerMotion.position + playerMotion.offset_to_ground;

                    vec2 diff = playerFootPos - footPos;
                    float distToPlayer = length(diff);
                    // If the player is still far away, force path recalculation.
                    auto& aiComp = registry.get<AIComponent>(entity);
                    const AIConfig& config = aiComp.stateMachine->getConfig();
                    if (distToPlayer > config.attackRange) {
                        pathRecalcTimer = pathRecalcInterval; // Force recalculation next update.

                        // std::cout << "v = 0 (1)" << std::endl;
                        motion.velocity = {0, 0};
                        return;
                    }
                }

                // std::cout << "v = 0 (2)" << std::endl;
                motion.velocity = {0, 0};
                return;
            }
        } else {
            vec2 direction = toWaypoint / distance;
            // Retrieve chaseSpeed from the shared configuration.
            auto& aiComp = registry.get<AIComponent>(entity);
            const AIConfig& config = aiComp.stateMachine->getConfig();

            // add some randomness to the mob speed to differentiate mob groups
            motion.velocity = dist(gen) * direction * config.chaseSpeed;
        }
    } else {
        // if no valid path is available
        // std::cout << "v = 0 (3)" << std::endl;

        motion.velocity = {0, 0};
    }
}

void ChaseState::onExit(entt::registry& registry, entt::entity entity) {
    // Stop movement on exit
    auto& motion = registry.get<Motion>(entity);
    // std::cout << "v = 0 (4)" << std::endl;

    motion.velocity = {0, 0};
    currentPath.clear();
    currentWaypointIndex = 0;
    pathRecalcTimer = 0.0f;
}