#pragma once
#include <ai/state_machine/state_factory.hpp>
#include <ai/state_machine/transition.hpp>

void initializeAIStates(StateFactory& stateFactory);

// const TransitionTable& getGoblinTransitionTable();

// AIConfig getGoblinAIConfig();

AIConfig getBossAIConfig();