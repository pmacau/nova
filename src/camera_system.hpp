#pragma once
#include "common.hpp"
#include "world_system.hpp"
#include <entt.hpp>
#include <cassert>
#include <sstream>
#include <iostream>


class CameraSystem {
public:
    CameraSystem(entt::registry& reg);  

    void step(float elapsed_ms);

private:
    entt::registry& registry;
    // WorldSystem& world;  
};