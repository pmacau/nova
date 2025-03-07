#pragma once

#include "ai_state.hpp"
#include <entt.hpp>

class AIStateMachine {
public:
    AIStateMachine(entt::registry& registry, entt::entity entity)
        : registry(registry), entity(entity), currentState(nullptr) {}

    // Update the current state.
    void update(float deltaTime) {
        if (currentState)
            currentState->onUpdate(registry, entity, deltaTime);
    }

    // Transition to a new state.
    void changeState(AIState* newState) {
        if (currentState) {
            currentState->onExit(registry, entity);
        }
        currentState = newState;
        if (currentState) {
            currentState->onEnter(registry, entity);
        }
    }

    AIState* getCurrentState() const { return currentState; }

private:
    entt::registry& registry;
    entt::entity entity;
    AIState* currentState;
};