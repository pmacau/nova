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

static Pathfinder g_pathFinder;

void ChaseState::onEnter(entt::registry& registry, entt::entity entity) {
    debug_printf(DebugType::AI, "ChaseState: onEnter, finding path to player\n");
    // Reset path-related variables.
    currentPath.clear();
    currentWaypointIndex = 0;
    pathRecalcTimer = 0.0f;
    
    // Compute the initial path.
    auto& motion = registry.get<Motion>(entity);
    
    // Retrieve player position.
    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0) return;

    auto playerEntity = *playerView.begin();
    auto& playerMotion = registry.get<Motion>(playerEntity);
    
    // Convert enemy and player world positions to tile indices.
    ivec2 enemyTile = ivec2(MapSystem::get_tile_indices(motion.position));
    ivec2 playerTile = ivec2(MapSystem::get_tile_indices(playerMotion.position));
    
    currentPath = g_pathFinder.findPath(enemyTile, playerTile);
    currentWaypointIndex = 0;
    debug_printf(DebugType::AI, "ChaseState: path found\n");

}

void ChaseState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    auto& motion = registry.get<Motion>(entity);
    
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
            ivec2 enemyTile = ivec2(MapSystem::get_tile_indices(motion.position));
            ivec2 playerTile = ivec2(MapSystem::get_tile_indices(playerMotion.position));
            currentPath = g_pathFinder.findPath(enemyTile, playerTile);
            currentWaypointIndex = 0;
        }
    }
    
    // Follow the path if available.
    if (!currentPath.empty() && currentWaypointIndex < currentPath.size()) {
        // The first node should be the enemy's current tile.
        // Use the next waypoint as target.
        ivec2 nextTile = currentPath[currentWaypointIndex];
        // Convert tile index to world coordinates (center of tile).
        vec2 nextWaypoint = MapSystem::get_tile_center_pos(vec2(nextTile.x, nextTile.y));
        
        vec2 toWaypoint = nextWaypoint - motion.position;
        float distance = std::sqrt(toWaypoint.x * toWaypoint.x + toWaypoint.y * toWaypoint.y);
        const float waypointThreshold = 5.0f; // Threshold in world units.
        
        if (distance < waypointThreshold) {
            // Waypoint reached; move to next.
            currentWaypointIndex++;
            if (currentWaypointIndex >= currentPath.size()) {
                // Reached final waypoint; stop movement.
                motion.velocity = {0, 0};
                return;
            }
        } else {
            vec2 direction = toWaypoint / distance;
            // Retrieve chaseSpeed from the shared configuration.
            auto& aiComp = registry.get<AIComponent>(entity);
            const AIConfig& config = aiComp.stateMachine->getConfig();
            motion.velocity = direction * config.chaseSpeed;
        }
    } else {
        // if no valid path is available
        motion.velocity = {0, 0};
    }
}

void ChaseState::onExit(entt::registry& registry, entt::entity entity) {
    // Stop movement on exit
    auto& motion = registry.get<Motion>(entity);
    motion.velocity = {0, 0};
    currentPath.clear();
    currentWaypointIndex = 0;
    pathRecalcTimer = 0.0f;
}