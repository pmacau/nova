#include "attack_state.hpp"
#include "ai/ai_component.hpp"
#include "ai/state_machine/idle_state.hpp"
#include "tinyECS/components.hpp"
#include <iostream>
#include <animation/animation_component.hpp>
#include <animation_system.hpp>
#include <animation/animation_manager.hpp>
#include <animation/animation_definition.hpp>

void AttackState::onEnter(entt::registry& registry, entt::entity entity) {
    // animation
    if (registry.any_of<AnimationComponent>(entity)) {
        auto& animComp = registry.get<AnimationComponent>(entity);
        AnimationSystem::setAnimationAction(animComp, MotionAction::ATTACK);

        // set state duration (same as animation duration)
        const std::string animationKey = AnimationManager::buildAnimationKey(
            animComp.animation_header, MotionAction::ATTACK, animComp.direction);
        const AnimationDefinition* animation_def = AnimationManager::getInstance().getAnimation(animationKey);
        if (animation_def) {
            stateDuration = 0.0f;
            for (const auto& duration : animation_def->frameDurations) {
                stateDuration += duration;
            }
            animComp.timer = 0.0f; // Reset the timer
            animComp.currentFrameIndex = 0; // Reset the frame index
        } else {
            std::cerr << "AttackState: Animation definition not found for key: " << animationKey << "\n";
            stateDuration = 0.0f; // Default to 0 if animation not found
        }
    }

    
}

void AttackState::onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) {
    stateTimer += deltaTime;

    auto& aiComp = registry.get<AIComponent>(entity);
    const AIConfig& config = aiComp.stateMachine->getConfig();

    // move towards player
    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0) {
        std::cout << "AttackState: No player found\n";
        return;
    }
    auto playerEntity = *playerView.begin();
    auto& playerMotion = registry.get<Motion>(playerEntity);
    vec2 playerFootPos = playerMotion.position + playerMotion.offset_to_ground;


    auto& motion = registry.get<Motion>(entity);
    vec2 footPos = motion.position + motion.offset_to_ground;

    vec2 diff = playerFootPos - footPos;
    vec2 direction = normalize(diff);
    motion.velocity = direction * config.chaseSpeed;
}

void AttackState::onExit(entt::registry& registry, entt::entity entity) {
    if (registry.any_of<AIComponent>(entity)) {
        auto& aiComp = registry.get<AIComponent>(entity);
        aiComp.attackCooldownTimer = 0.0f;
    }

    auto& aiComp = registry.get<AIComponent>(entity);

}