
#include <tinyECS/components.hpp>

#include "ai_initializer.hpp"
#include <ai/state_machine/idle_state.hpp>
#include <ai/state_machine/chase_state.hpp>
#include <ai/state_machine/patrol_state.hpp>
#include <ai/state_machine/attack_state.hpp>
#include <ai/state_machine/retreat_state.hpp>
#include <ai/state_machine/basic_range/range_attack_state.hpp>

void initializeAIStates(StateFactory& stateFactory) {
    stateFactory.registerState("idle", []() { return std::make_unique<IdleState>(); });
    stateFactory.registerState("chase", []() { return std::make_unique<ChaseState>(); });
    stateFactory.registerState("patrol", []() { return std::make_unique<PatrolState>(); });
    stateFactory.registerState("attack", []() { return std::make_unique<AttackState>(); });
    stateFactory.registerState("retreat", []() { return std::make_unique<RetreatState>(); });
    stateFactory.registerState("range_attack", []() { return std::make_unique<RangeAttackState>(); });
}
