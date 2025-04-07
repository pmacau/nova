#pragma once

#include <iostream>
#include <random>

#include "ai/ai_component.hpp"
#include "tinyECS/components.hpp"

#include <ai/state_machine/basic_range/basic_range_common.hpp>


class RangeAttackState : public AIState
{
public:
    RangeAttackState() : shotsRemaining(0), shotTimer(0.0f), stateComplete(false) { id = "range_attack"; }
    virtual void onEnter(entt::registry &registry, entt::entity entity) override;
    virtual void onUpdate(entt::registry &registry, entt::entity entity, float deltaTime) override;
    virtual void onExit(entt::registry &registry, entt::entity entity) override;

    virtual bool isStateComplete() const override { return stateComplete; }

private:
    int shotsRemaining;
    float shotTimer;
    bool stateComplete; // Flag to indicate that the attack sequence is finished.

    void shootProjectile(entt::registry &registry, entt::entity entity, const RangeAIConfig &config);


    std::default_random_engine rng;
};