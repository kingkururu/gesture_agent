#pragma once

#include <ApplicationServices/ApplicationServices.h>
#include <IOKit/hidsystem/ev_keymap.h>  
#include <CoreGraphics/CoreGraphics.h>

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

#include "core/gesture_types.h"
#include "utils/log.hpp"

namespace macos {
    void performGestureAction(GestureType gesture);

    // Concrete actions
    void click();
    void swipeLeft();
    void swipeRight();
}
