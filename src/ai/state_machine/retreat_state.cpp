#include "retreat_state.hpp"
#include "ai/ai_component.hpp"
#include "ai/ai_common.hpp"         // For RangeAIConfig.
#include "tinyECS/components.hpp"   // For Motion, etc.
#include "ai/state_machine/transition.hpp"
#include <map/map_system.hpp>
#include "ai/path_finder.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <algorithm>
#include <random>

ivec2 RetreatState::computeRetreatDestination(entt::registry& registry, entt::entity entity, float retreatDistance) {
    // Get enemy and player positions.
    auto& motion = registry.get<Motion>(entity);
    vec2 enemyFootPos = motion.position + motion.offset_to_ground;
    
    auto playerView = registry.view<Player, Motion>();

    if (playerView.size_hint() == 0) {
        // Fallback: return enemy's current tile.
        return ivec2(MapSystem::get_tile_indices(enemyFootPos));
    }
    auto playerEntity = *playerView.begin();
    auto& playerMotion = registry.get<Motion>(playerEntity);
    vec2 playerFootPos = playerMotion.position + playerMotion.offset_to_ground;
    
    // Compute the vector from player to enemy (opposite of player direction).
    vec2 retreatDir = normalize(enemyFootPos - playerFootPos);
    // Extend the retreat direction by retreatDistance.
    vec2 candidatePos = enemyFootPos + retreatDir * retreatDistance;
    
    // Convert candidate world position to tile indices.
    ivec2 candidateTile = ivec2(MapSystem::get_tile_indices(candidatePos));
    
    // Check if candidate tile is walkable.
    if (MapSystem::walkable_tile(MapSystem::get_tile(MapSystem::get_tile_center_pos(vec2(candidateTile.x, candidateTile.y))))) {
        return candidateTile;
    }
    
    // If not, search in a small radius around candidateTile.
    const int searchRadius = 10;
    for (int dy = -searchRadius; dy <= searchRadius; ++dy) {
        for (int dx = -searchRadius; dx <= searchRadius; ++dx) {
            ivec2 testTile = candidateTile + ivec2(dx, dy);
            // Ensure testTile is within bounds.
            if (testTile.x < 0 || testTile.x >= MapSystem::map_width ||
                testTile.y < 0 || testTile.y >= MapSystem::map_height)
                continue;
            vec2 testCenter = MapSystem::get_tile_center_pos(vec2(testTile.x, testTile.y));
            if (MapSystem::walkable_tile(MapSystem::get_tile(testCenter))) {
                return testTile;
            }
        }
    }
    // Fallback: if no valid tile found, return enemy's current tile.
    return ivec2(MapSystem::get_tile_indices(enemyFootPos));
}

void RetreatState::regenerateRetreatPath(entt::registry& registry, entt::entity entity, ivec2 startTile, ivec2 targetTile) {
    retreatPath = Pathfinder::findPath(startTile, targetTile);
    if (!retreatPath.empty()) {
        // Optionally remove the first tile if it is the enemy's current tile.
        retreatPath.erase(retreatPath.begin());
    }
    currentWaypointIndex = 0;
    pathRecalcTimer = 0.0f;
    // Debug: you could call createDebugTile(registry, tile) for each tile in retreatPath.
}

void RetreatState::onEnter(entt::registry& registry, entt::entity entity) {
    std::cout << "Entering RetreatState\n";
    // Get the RangeAIConfig.
    auto& aiComp = registry.get<AIComponent>(entity);
    const AIConfig& config = static_cast<const AIConfig&>(aiComp.stateMachine->getConfig());
    
    // Compute enemy's current tile.
    auto& motion = registry.get<Motion>(entity);
    vec2 footPos = motion.position + motion.offset_to_ground;
    ivec2 enemyTile = ivec2(MapSystem::get_tile_indices(footPos));
    
    // Compute a retreat destination using the configured retreatDistance.
    ivec2 retreatTile = computeRetreatDestination(registry, entity, config.retreatDistance);
    
    // Regenerate path from enemy tile to retreat tile.
    regenerateRetreatPath(registry, entity, enemyTile, retreatTile);
}

void RetreatState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    auto& motion = registry.get<Motion>(entity);
    vec2 footPos = motion.position + motion.offset_to_ground;
    ivec2 currentTile = ivec2(MapSystem::get_tile_indices(footPos));
    
    // Update path recalculation timer.
    pathRecalcTimer += deltaTime;
    const float pathRecalcInterval = 1000.0f; // Recalculate path every 1000ms.
    if (pathRecalcTimer >= pathRecalcInterval) {
        pathRecalcTimer = 0.0f;
        // Recompute retreat destination and path.
        auto& aiComp = registry.get<AIComponent>(entity);
        const AIConfig& config = static_cast<const AIConfig&>(aiComp.stateMachine->getConfig());
        ivec2 retreatTile = computeRetreatDestination(registry, entity, config.retreatDistance);
        regenerateRetreatPath(registry, entity, currentTile, retreatTile);
    }
    
    // Follow the retreat path if available.
    if (!retreatPath.empty() && currentWaypointIndex < static_cast<int>(retreatPath.size())) {
        ivec2 nextTile = retreatPath[currentWaypointIndex];
        vec2 nextWaypoint = MapSystem::get_tile_center_pos(vec2(nextTile.x, nextTile.y));
        
        vec2 toWaypoint = nextWaypoint - footPos;
        float distance = length(toWaypoint);
        const float threshold = TILE_SIZE * 0.5f;
        if (distance < threshold) {
            currentWaypointIndex++;
            if (currentWaypointIndex >= static_cast<int>(retreatPath.size())) {
                // Reached retreat destination.
                stateComplete = true;
                motion.velocity = {0, 0};
                return;
            }
        } else {
            vec2 direction = normalize(toWaypoint);
            // Use chaseSpeed (or define a separate retreatSpeed) from the AI config.
            auto& aiComp = registry.get<AIComponent>(entity);
            const AIConfig& config = static_cast<const AIConfig&>(aiComp.stateMachine->getConfig());
            motion.velocity = direction * config.chaseSpeed;
        }
    } else {
        motion.velocity = {0, 0};
    }
}

void RetreatState::onExit(entt::registry& registry, entt::entity entity) {
    std::cout << "Exiting RetreatState\n";
    auto& motion = registry.get<Motion>(entity);
    motion.velocity = {0, 0};
    retreatPath.clear();
    currentWaypointIndex = 0;
    pathRecalcTimer = 0.0f;
}