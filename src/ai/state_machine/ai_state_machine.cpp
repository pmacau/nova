#include "ai_state_machine.hpp"
#include "state_factory.hpp"
#include <ai/state_machine/transition.hpp>

AIStateMachine::AIStateMachine(entt::registry &registry, entt::entity entity, std::shared_ptr<AIConfig> cfg, const TransitionTable &transitions)
    : registry(registry), entity(entity), currentState(nullptr), config(std::move(cfg)), transitions(transitions)
{
}

AIStateMachine::~AIStateMachine()
{
    // If needed, clean up currentState here. Typically states are shared or managed externally.
}

void AIStateMachine::update(float deltaTime)
{
    if (currentState)
    {
        currentState->onUpdate(registry, entity, deltaTime);

        // check if transition is needed
        std::string currentStateId = currentState->getId();

        auto it = transitions.find(currentStateId);
        if (it != transitions.end())
        {
            for (const auto &transition : it->second)
            {
                if (transition.condition(registry, entity, *config, currentState))
                {

                    // Use the StateFactory to create a new state instance
                    // TODO: refactor state factory to be a singleton
                    std::unique_ptr<AIState> newState = g_stateFactory.createState(transition.targetStateId);
                    if (newState)
                    {
                        changeState(newState.release());
                    }
                    // only transition to the first matching state (TODO: could change later for a probabilistic transition)
                    break;
                }
            }
        }
    }
}

void AIStateMachine::changeState(AIState *newState)
{
    if (currentState)
    {
        currentState->onExit(registry, entity);
    }
    currentState = newState;
    if (currentState)
    {
        currentState->onEnter(registry, entity);
    }
}

void AIStateMachine::changeStateByStateId(const std::string &stateId)
{
    // Use the StateFactory to create a new state instance
    std::unique_ptr<AIState> newState = g_stateFactory.createState(stateId);
    if (newState)
    {
        changeState(newState.release());
    }
}

AIState *AIStateMachine::getCurrentState() const
{
    return currentState;
}