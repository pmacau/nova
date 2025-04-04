#pragma once
#include <ai/ai_common.hpp>

inline AIConfig getBossAIConfig() {
    AIConfig config;
    config.detectionRange = 500.0f;
    config.unchaseRange = 800.0f;
    config.chaseSpeed = 160.0f;
    config.attackRange = 50.0f;
    return config;
}