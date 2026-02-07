#pragma once

#include <ApplicationServices/ApplicationServices.h>

#include "core/gesture_types.h"
#include "utils/log.hpp"

namespace macos {
    void performGestureAction(GestureType gesture);

    // Concrete actions
    void click();
    void swipeLeft();
    void swipeRight();
}
