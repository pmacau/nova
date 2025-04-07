#pragma once

#include "state_machine/ai_state_machine.hpp"
#include <memory>

struct AIComponent {
    std::unique_ptr<AIStateMachine> stateMachine;
    float attackCooldownTimer = 0.0f;
};