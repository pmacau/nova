#pragma once
#include <ai/state_machine/transition.hpp>
#include <tinyECS/components.hpp>
#include <ai/state_machine/ai_state.hpp>

inline const TransitionTable& getBasicFighterTransitionTable() {
    static TransitionTable basicFighterTransitions;
    if (basicFighterTransitions.empty()) {
        // Transition from "patrol" to "chase"
        basicFighterTransitions["patrol"].push_back({
            "chase",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
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
        basicFighterTransitions["chase"].push_back({
            "attack",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
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
        basicFighterTransitions["chase"].push_back({
            "patrol",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
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
        basicFighterTransitions["attack"].push_back({
            "chase",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                float dist = length(diff);
                return currState->isStateComplete() && (dist >= config.attackRange && dist < config.unchaseRange);
            }
        });

        // Transition from "attack" to "patrol"
        basicFighterTransitions["attack"].push_back({
            "patrol",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                return currState->isStateComplete() && (length(diff) > config.unchaseRange);
            }
        });

        basicFighterTransitions["attack"].push_back({
            "attack",
            [](entt::registry& reg, entt::entity entity, const AIConfig& config, AIState* currState) -> bool {
                auto playerView = reg.view<Player, Motion>();
                if (playerView.size_hint() == 0) return false;
                auto playerEntity = *playerView.begin();
                auto& playerMotion = reg.get<Motion>(playerEntity);
                auto& motion = reg.get<Motion>(entity);
                vec2 diff = playerMotion.position - motion.position;
                return currState->isStateComplete() && (length(diff) < config.attackRange);
            }
        });

    }
    return basicFighterTransitions;
}