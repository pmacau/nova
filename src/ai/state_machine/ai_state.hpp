#pragma once

#include <entt.hpp>
#include "common.hpp"

// abstract base class for FSM states 
class AIState {
public:
    virtual ~AIState() = default;

    // Called when this state is entered.
    virtual void onEnter(entt::registry& registry, entt::entity entity) = 0;

    // Called each frame to update state behavior.
    virtual void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) = 0;

    // Called when this state is exited.
    virtual void onExit(entt::registry& registry, entt::entity entity) = 0;

    // handle events (for later)
    virtual void onEvent(entt::registry& registry, entt::entity entity, const std::string &event) {}

    virtual std::string getId() const { return id; }

protected:
    std::string id;
};