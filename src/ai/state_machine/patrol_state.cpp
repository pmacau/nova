#include "patrol_state.hpp"
#include "tinyECS/components.hpp"
#include <iostream>
#include <cmath>
#include <random>
#include "chase_state.hpp"
#include <ai/ai_component.hpp>
#include "attack_state.hpp"
#include <map/map_system.hpp>
#include <animation_system.hpp>


// Helper: Attempt to find a valid patrol target given current position and patrol radius.
// Returns a candidate world position that is in a walkable tile.
// Note: currentPos need to be the foot position of the entity.
static glm::vec2 findValidPatrolTarget(const glm::vec2& currentPos, float patrolRadius, std::default_random_engine& rng) {
    const int maxAttempts = 5;
    for (int i = 0; i < maxAttempts; ++i) {
        std::uniform_real_distribution<float> angleDist(0.0f, 2 * 3.14159265f);
        float angle = angleDist(rng);
        glm::vec2 direction = { std::cos(angle), std::sin(angle) };

        // Determine the maximum valid distance along that direction
        // const ivec2 currentPosTileIndices = MapSystem::get_tile_indices(currentPos);
        const float stepSize = TILE_SIZE / 2.f; // TODO: make this a parameter?
        float maxValidDistance = 0.0f;
        for (float d = 0.0f; d <= patrolRadius; d += stepSize) {
            glm::vec2 candidate = currentPos + direction * d;

            glm::vec2 candidateIndices = MapSystem::get_tile_indices(candidate);
            int tileX = static_cast<int>(candidateIndices.x);
            int tileY = static_cast<int>(candidateIndices.y);
            // If the candidate tile is not walkable, break out.
            if (!MapSystem::walkable_tile(MapSystem::get_tile_type_by_indices(tileX, tileY))) {
                break;
            }
            maxValidDistance = d;
        }

        if (maxValidDistance > 0.0f) {
            // random distance within the valid range
            std::uniform_real_distribution<float> distDist(0.0f, maxValidDistance);
            float chosenDistance = distDist(rng);
            return currentPos + direction * chosenDistance;
        }
    }
    // no valid candidate was found
    return currentPos;
}

PatrolState::PatrolState()
    : patrolTarget({0.f, 0.f})
{
    std::random_device rd;
    rng.seed(rd());
    id = "patrol";
}

void PatrolState::onEnter(entt::registry& registry, entt::entity entity) {
    auto& motion = registry.get<Motion>(entity);
    auto& aiComp = registry.get<AIComponent>(entity);
    const AIConfig& config = aiComp.stateMachine->getConfig();
    
    // find a valid patrol target
    vec2 footPos = motion.position + motion.offset_to_ground;
    patrolTarget = findValidPatrolTarget(footPos, config.patrolRadius, rng);

    // animtion
    if (registry.any_of<AnimationComponent>(entity)) {
        auto& animComp = registry.get<AnimationComponent>(entity);
        AnimationSystem::setAnimationAction(animComp, MotionAction::IDLE);
    }

}

void PatrolState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    auto& motion = registry.get<Motion>(entity);
    auto& aiComp = registry.get<AIComponent>(entity);
    const AIConfig& config = aiComp.stateMachine->getConfig();

    vec2 footPos = motion.position + motion.offset_to_ground;

    // Compute the vector toward the current patrol target.
    vec2 toTarget = patrolTarget - footPos;
    float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

    if (distance < config.patrolThreshold) {
        // Target reached
        patrolTarget = findValidPatrolTarget(footPos, config.patrolRadius, rng);

        motion.velocity = {0, 0};
    } else {
        // Move toward the patrol target
        vec2 direction = toTarget / distance;
        motion.velocity = direction * config.patrolSpeed;
    }
}

void PatrolState::onExit(entt::registry& registry, entt::entity entity) {
    auto& motion = registry.get<Motion>(entity);
    motion.velocity = {0.f, 0.f};
}