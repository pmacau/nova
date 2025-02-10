#pragma once

#include "common.hpp"
#include "render_system.hpp"
#include <entt.hpp>

class AISystem {
public:
    AISystem(entt::registry& reg);
    void step(float elapsed_ms);

private:
    entt::registry& registry;
};