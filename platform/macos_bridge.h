#pragma once

#include "core/gesture_types.h"

namespace macos {
    void performGestureAction(GestureType gesture);

    // Concrete actions
    void click();
    void swipeLeft();
    void swipeRight();
}
