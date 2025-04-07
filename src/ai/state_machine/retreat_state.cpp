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

#include <animation/animation_component.hpp>
#include <animation_system.hpp>

ivec2 RetreatState::computeRetreatDestination(entt::registry& registry, entt::entity entity, float retreatDistance) {
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
    
    vec2 retreatDir = normalize(enemyFootPos - playerFootPos);
    vec2 candidatePos = enemyFootPos + retreatDir * retreatDistance;
    
    ivec2 candidateTile = ivec2(MapSystem::get_tile_indices(candidatePos));

    // Use a ray-cast approach: move from candidateTile along the retreat direction until an obstacle is found.
    ivec2 currentTile = candidateTile;
    ivec2 step = ivec2(
        (retreatDir.x > 0) ? 1 : ((retreatDir.x < 0) ? -1 : 0),
        (retreatDir.y > 0) ? 1 : ((retreatDir.y < 0) ? -1 : 0)
    );
    
    // Limit the number of steps to avoid infinite loops.
    const int maxSteps = 20;
    for (int i = 0; i < maxSteps; ++i) {
        vec2 center = MapSystem::get_tile_center_pos(vec2(currentTile.x, currentTile.y));
        if (!MapSystem::walkable_tile(MapSystem::get_tile(center))) {
            // Step back one tile.
            currentTile = currentTile - step;
            break;
        }
        currentTile = currentTile + step;
    }
    
    return currentTile;
}

void RetreatState::regenerateRetreatPath(entt::registry& registry, entt::entity entity, ivec2 startTile, ivec2 targetTile) {
    retreatPath = Pathfinder::findPath(startTile, targetTile, true);
    if (!retreatPath.empty()) {
        // Optionally remove the first tile if it is the enemy's current tile.
        retreatPath.erase(retreatPath.begin());
    }
    currentWaypointIndex = 0;
    pathRecalcTimer = 0.0f;
}

void RetreatState::onEnter(entt::registry& registry, entt::entity entity) {
    debug_printf(DebugType::AI, "Entering RetreatState\n");

    stateComplete = false;
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

    if (registry.any_of<AnimationComponent>(entity)) {
        auto& animComp = registry.get<AnimationComponent>(entity);
        AnimationSystem::setAnimationAction(animComp, MotionAction::WALK);
    }

}

void RetreatState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    auto& motion = registry.get<Motion>(entity);
    vec2 footPos = motion.position + motion.offset_to_ground;
    ivec2 currentTile = ivec2(MapSystem::get_tile_indices(footPos));
    
    // Update path recalculation timer.
    pathRecalcTimer += deltaTime;
    const float pathRecalcInterval = 2000.0f; // Recalculate path every 2000ms.
    if (pathRecalcTimer >= pathRecalcInterval) {
        stateComplete = true;

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
    debug_printf(DebugType::AI, "Exiting RetreatState\n");
    auto& motion = registry.get<Motion>(entity);
    motion.velocity = {0, 0};
    retreatPath.clear();
    currentWaypointIndex = 0;
    pathRecalcTimer = 0.0f;
}