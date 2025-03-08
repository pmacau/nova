#include "ai_state_machine.hpp"

AIStateMachine::AIStateMachine(entt::registry& registry, entt::entity entity)
    : registry(registry), entity(entity), currentState(nullptr)
{
}

AIStateMachine::~AIStateMachine() {
    // If needed, clean up currentState here. Typically states are shared or managed externally.
}

void AIStateMachine::update(float deltaTime) {
    if (currentState) {
        currentState->onUpdate(registry, entity, deltaTime);
    }
}

void AIStateMachine::changeState(AIState* newState) {
    if (currentState) {
        currentState->onExit(registry, entity);
    }
    currentState = newState;
    if (currentState) {
        currentState->onEnter(registry, entity);
    }
}

AIState* AIStateMachine::getCurrentState() const {
    return currentState;
}