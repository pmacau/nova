#pragma once

#include <iostream>

#include <ai/state_machine/basic_range/basic_range_common.hpp>
#include "ai/state_machine/transition.hpp"
#include "tinyECS/components.hpp"
#include "ai/ai_common.hpp"
#include "ai/state_machine/ai_state.hpp"
#include <cmath>
#include <ai/ai_component.hpp>

inline bool shouldTransitionToChase(float diff, const AIConfig& config) {
    return (diff < config.detectionRange) && (diff > config.attackRange) && (diff > config.retreatRange);
}

inline bool shouldTransitionToPatrol(float diff, const AIConfig& config) {
    return (diff > config.unchaseRange);
}

inline bool shouldTransitionToAttack(float diff, const RangeAIConfig& rangeConfig, float attackCooldownTimer) {
    return (diff < rangeConfig.attackRange) && (diff >= rangeConfig.retreatRange) && (attackCooldownTimer >= rangeConfig.attackCooldown);
}

inline bool shouldTransitionToRetreat(float diff, const AIConfig& config) {
    return (diff < config.retreatRange);
}

inline const TransitionTable& getBasicRangerTransitionTable() {
    static TransitionTable rangedTransitions;
    if (rangedTransitions.empty()) {
        // Transition from "patrol" to "chase" if player is detected.
        rangedTransitions["patrol"].push_back({
            "chase",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                float dist = length(playerMotion.position - motion.position);
                
                
                return shouldTransitionToChase(dist, config);
            }
        });

        rangedTransitions["patrol"].push_back({
            "range_attack",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                float dist = length(playerMotion.position - motion.position);
                
                auto& aiComp = reg.get<AIComponent>(entity);
                const RangeAIConfig& rangeConfig = static_cast<const RangeAIConfig&>(config);
                return shouldTransitionToAttack(dist, rangeConfig, aiComp.attackCooldownTimer);
            }
        });

        rangedTransitions["patrol"].push_back({
            "retreat",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                float dist = length(playerMotion.position - motion.position);
                
                
                return shouldTransitionToRetreat(dist, config);
            }
        });

        rangedTransitions["chase"].push_back({
            "patrol",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                return shouldTransitionToPatrol(length(diff), config);
            }
        });

        // Transition from "chase" to "retreat" if player is too close.
        rangedTransitions["chase"].push_back({
            "retreat",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                float dist = length(playerMotion.position - motion.position);
                return shouldTransitionToRetreat(dist, config);
            }
        });

        // Transition from "chase" to "attack" if player is within attack range (but not too close).
        rangedTransitions["chase"].push_back({
            "range_attack",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                float dist = length(playerMotion.position - motion.position);

                auto& aiComp = reg.get<AIComponent>(entity);
                const RangeAIConfig& rangeConfig = static_cast<const RangeAIConfig&>(config);
                return shouldTransitionToAttack(dist, rangeConfig, aiComp.attackCooldownTimer);
            }
        });

        // Transition from "attack" to "chase" if the attack sequence is complete and the player is no longer in attack range.
        

        rangedTransitions["range_attack"].push_back({
            "patrol",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                return currState->isStateComplete() && shouldTransitionToPatrol(length(diff), config);
            }
        });

        rangedTransitions["retreat"].push_back({
            "range_attack",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                float dist = length(playerMotion.position - motion.position);
                auto& aiComp = reg.get<AIComponent>(entity);
                const RangeAIConfig& rangeConfig = static_cast<const RangeAIConfig&>(config);
                return shouldTransitionToAttack(dist, rangeConfig, aiComp.attackCooldownTimer);
            }
        });

        rangedTransitions["retreat"].push_back({
            "chase",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                float dist = length(playerMotion.position - motion.position);
                return shouldTransitionToChase(dist, config);
            }
        });

        // rangedTransitions["range_attack"].push_back({
        //     "range_attack",
        //     [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
        //         auto playerView = reg.view<Player, Motion>();
        //         if (playerView.size_hint() == 0) return false;
        //         auto playerEntity = *playerView.begin();
        //         auto& playerMotion = reg.get<Motion>(playerEntity);
        //         auto& motion = reg.get<Motion>(entity);
        //         float dist = length(playerMotion.position - motion.position);

        //         auto& aiComp = reg.get<AIComponent>(entity);
        //         const RangeAIConfig& rangeConfig = static_cast<const RangeAIConfig&>(config);
                 
        //         return currState->isStateComplete() && shouldTransitionToAttack(dist, rangeConfig, aiComp.attackCooldownTimer);
        //     }
        // });

        rangedTransitions["range_attack"].push_back({
            "chase",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                float dist = length(playerMotion.position - motion.position);
                return currState->isStateComplete();
            }
        });
    }
    return rangedTransitions;
}