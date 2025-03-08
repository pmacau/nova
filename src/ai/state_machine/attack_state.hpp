#pragma once

#include "ai_state.hpp"
#include <iostream>

class AttackState : public AIState {
public:
    void onEnter(entt::registry& registry, entt::entity entity) override;
    void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override;
    void onExit(entt::registry& registry, entt::entity entity) override;
};