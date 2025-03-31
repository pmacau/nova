#pragma once


struct FrameIndex {
    int row;
    int col;
};

enum class MotionAction {
    IDLE,
    WALK,
    ATTACK,
    DASH,
    HURT,
    DEATH
};

enum class MotionDirection {
    UP,
    DOWN,
    RIGHT
};