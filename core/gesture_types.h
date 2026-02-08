#pragma once

enum class GestureType {
    NONE,

    // Poses
    OPEN_PALM,
    FIST,
    POINT,
    THUMBS_UP,
    PINCH,

    // Motion
    SWIPE_LEFT,
    SWIPE_RIGHT,
    SWIPE_UP,
    SWIPE_DOWN,

    // States
    PINCH_HOLD,
    PINCH_RELEASE,
};
