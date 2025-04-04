#pragma once
#include <string>
#include <functional>
#include <entt.hpp>
#include <ai/ai_common.hpp> // For AIConfig definition
#include <ai/state_machine/ai_state.hpp>

struct Transition {
    std::string targetStateId;
    std::function<bool(entt::registry&, entt::entity, const AIConfig&, AIState* currState)> condition; // (...) => bool (transition or not)
};

// state ID (string) -> list of Transitions.
using TransitionTable = std::unordered_map<std::string, std::vector<Transition>>;