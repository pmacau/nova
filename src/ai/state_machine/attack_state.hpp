#pragma once

#include "ai_state.hpp"
#include <iostream>

class AttackState : public AIState {
public:
    AttackState() {
        id = "attack";
    }

    void onEnter(entt::registry& registry, entt::entity entity) override;
    void onUpdate(entt::registry& registry, entt::entity entity, float deltaTime) override;
    void onExit(entt::registry& registry, entt::entity entity) override;

    bool isStateComplete() const override {
        return stateTimer >= stateDuration;
    }

private:
    float stateDuration = 0.0f;
    float stateTimer = 0.0f;
};