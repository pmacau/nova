#pragma once

#include "ai_state.hpp"
#include <entt.hpp>
#include <ai/state_machine/transition.hpp>

class AIStateMachine {
public:
    // Constructor: takes a reference to the registry and the entity owning this state machine.
    AIStateMachine(entt::registry& registry, entt::entity entity, std::shared_ptr<AIConfig> config, const TransitionTable& transitions);

    // Destructor.
    ~AIStateMachine();

    // Update the current state.
    void update(float deltaTime);

    // Change the current state.
    void changeState(AIState* newState);

    // Get the current state.
    AIState* getCurrentState() const;

    const AIConfig& getConfig() const { return *config; }

private:
    entt::registry& registry;
    entt::entity entity;
    AIState* currentState;

    std::shared_ptr<AIConfig> config;

    TransitionTable transitions;
};