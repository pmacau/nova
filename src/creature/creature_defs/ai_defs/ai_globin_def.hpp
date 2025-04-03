#pragma once
#include <ai/ai_common.hpp>

inline AIConfig getGoblinAIConfig() {
    AIConfig config;

    config.detectionRange = 300.0f;
    config.unchaseRange = 500.0f;
    config.chaseSpeed = 100.0f;
    config.attackRange = 80.0f;
    config.patrolRadius = 100.0f;
    config.patrolSpeed = 25.f;
    config.patrolThreshold = 5.0f;
    return config;
}