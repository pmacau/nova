#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "ai_state.hpp"

// A factory that produces AIState instances based on a state ID.
class StateFactory {
public:
    // () => *AIState
    using CreatorFunc = std::function<std::unique_ptr<AIState>()>;

    // Registers a state creation function with a unique ID.
    void registerState(const std::string& id, CreatorFunc creator);

    // Creates a new state instance by its ID.
    std::unique_ptr<AIState> createState(const std::string& id) const;

private:

    std::unordered_map<std::string, CreatorFunc> creators;
};

extern StateFactory g_stateFactory;