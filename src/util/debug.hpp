#pragma once

#include <iostream>
#include <cstdarg>
#include <cstdio>

#include <vector>
#include <unordered_map>
#include <string>

// Change this flag to false if your terminal does not support ANSI colored text
const bool ENABLE_COLOR = true;
const std::string start_color = "\033[";
const std::string stop_color = "\033[0m";

struct DebugData {
    bool enabled;
    int ansi_color;
    const std::string header;
};

enum class DebugType {
    WORLD_INIT, WORLD, PHYSICS, COLLISION, GAME_INIT,
    SPAWN,
    USER_INPUT,
    HITBOX,
    FLAG,
    DEBUG_TYPE_COUNT
};

inline std::unordered_map<DebugType, DebugData> debug_options = {
    // Systems

    {WORLD_INIT, {true, 32, "[WORLD INIT]"}},
    {WORLD,      {true, 34, "[WORLD]"}},
    {PHYSICS,    {true, 35, "[PHYSICS]"}},
    {COLLISION,  {true, 33, "[COLLISION]"}},
    {GAME_INIT,  {true, 31, "[GAME INIT]"}},
    {HITBOX,  {true, 36, "[HITBOX]"}},
    {FLAG, {true, 38, "[FLAG]"}},
    // Fine-grain debug types
    {USER_INPUT, {true, 37, "[INPUT]"}}

};

inline void toggle_debug(DebugType type) {
    if (debug_options.find(type) != debug_options.end()) {
        debug_options[type].enabled = !debug_options[type].enabled;
    }
}

inline void debug_printf(DebugType type, const char *format, ...) {
    if (
        debug_options.find(type) != debug_options.end() &&
        debug_options[type].enabled
    ) {
        if (ENABLE_COLOR) {
            auto& data = debug_options[type];
            std::cout << start_color << data.ansi_color << "m";
            std::cout << data.header << stop_color << " ";
        }
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
};