#include "ai_initializer.hpp"
#include <ai/state_machine/idle_state.hpp>
#include <ai/state_machine/chase_state.hpp>
#include <ai/state_machine/patrol_state.hpp>
#include <ai/state_machine/attack_state.hpp>
#include <tinyECS/components.hpp>

void initializeAIStates(StateFactory& stateFactory) {
    stateFactory.registerState("idle", []() { return std::make_unique<IdleState>(); });
    stateFactory.registerState("chase", []() { return std::make_unique<ChaseState>(); });
    stateFactory.registerState("patrol", []() { return std::make_unique<PatrolState>(); });
    stateFactory.registerState("attack", []() { return std::make_unique<AttackState>(); });
}

// const TransitionTable& getGoblinTransitionTable() {
//     static TransitionTable goblinTransitions;
//     if (goblinTransitions.empty()) {
//         // Transition from "patrol" to "chase"
//         goblinTransitions["patrol"].push_back({
//             "chase",
//             [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
//                 auto playerView = reg.view<Player, Motion>();
//                 if (playerView.size_hint() == 0) return false;
//                 auto playerEntity = *playerView.begin();
//                 auto& playerMotion = reg.get<Motion>(playerEntity);
//                 auto& motion = reg.get<Motion>(entity);
//                 vec2 diff = playerMotion.position - motion.position;
//                 return (length(diff) < config.detectionRange);
//             }
//         });
//         // Transition from "chase" to "attack"
//         goblinTransitions["chase"].push_back({
//             "attack",
//             [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
//                 auto playerView = reg.view<Player, Motion>();
//                 if (playerView.size_hint() == 0) return false;
//                 auto playerEntity = *playerView.begin();
//                 auto& playerMotion = reg.get<Motion>(playerEntity);
//                 auto& motion = reg.get<Motion>(entity);
//                 vec2 diff = playerMotion.position - motion.position;
//                 return (length(diff) < config.attackRange);
//             }
//         });
//         // Transition from "chase" back to "idle"
//         goblinTransitions["chase"].push_back({
//             "patrol",
//             [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
//                 auto playerView = reg.view<Player, Motion>();
//                 if (playerView.size_hint() == 0) return false;
//                 auto playerEntity = *playerView.begin();
//                 auto& playerMotion = reg.get<Motion>(playerEntity);
//                 auto& motion = reg.get<Motion>(entity);
//                 vec2 diff = playerMotion.position - motion.position;
//                 return (length(diff) > config.unchaseRange);
//             }
//         });
//         // Transition from "attack" to "chase"
//         goblinTransitions["attack"].push_back({
//             "chase",
//             [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
//                 auto playerView = reg.view<Player, Motion>();
//                 if (playerView.size_hint() == 0) return false;
//                 auto playerEntity = *playerView.begin();
//                 auto& playerMotion = reg.get<Motion>(playerEntity);
//                 auto& motion = reg.get<Motion>(entity);
//                 vec2 diff = playerMotion.position - motion.position;
//                 float dist = length(diff);
//                 return (dist >= config.attackRange && dist < config.unchaseRange);
//             }
//         });

//         // Transition from "attack" to "patrol"
//         goblinTransitions["attack"].push_back({
//             "patrol",
//             [](entt::registry& reg, entt::entity entity, const AIConfig& config) -> bool {
//                 auto playerView = reg.view<Player, Motion>();
//                 if (playerView.size_hint() == 0) return false;
//                 auto playerEntity = *playerView.begin();
//                 auto& playerMotion = reg.get<Motion>(playerEntity);
//                 auto& motion = reg.get<Motion>(entity);
//                 vec2 diff = playerMotion.position - motion.position;
//                 return (length(diff) > config.unchaseRange);
//             }
//         });

//     }
//     return goblinTransitions;
// }

// AIConfig getGoblinAIConfig() {
//     AIConfig config;

//     config.detectionRange = 300.0f;
//     config.unchaseRange = 500.0f;
//     config.chaseSpeed = 100.0f;
//     config.attackRange = 50.0f;
//     config.patrolRadius = 100.0f;
//     config.patrolSpeed = 25.f;
//     config.patrolThreshold = 5.0f;
//     return config;
// }

// AIConfig getBossAIConfig() {
//     AIConfig config;
//     config.detectionRange = 500.0f;
//     config.unchaseRange = 800.0f;
//     config.chaseSpeed = 160.0f;
//     config.attackRange = 50.0f;
//     return config;
// }