#include "macos_bridge.h"
#import <Cocoa/Cocoa.h>

namespace macos {

    void click() {
        CGEventRef locEvent = CGEventCreate(NULL);
        CGPoint pos = CGEventGetLocation(locEvent);
        CFRelease(locEvent);

        CGEventRef down = CGEventCreateMouseEvent(
            nullptr, kCGEventLeftMouseDown, pos, kCGMouseButtonLeft
        );
        CGEventRef up = CGEventCreateMouseEvent(
            nullptr, kCGEventLeftMouseUp, pos, kCGMouseButtonLeft
        );

        CGEventPost(kCGHIDEventTap, down);
        CGEventPost(kCGHIDEventTap, up);

        CFRelease(down);
        CFRelease(up);
    }

    void increase_brightness() {
        // Brightness up key code is 144 (0x90)
        CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, 144, true);
        CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, 144, false);
        
        CGEventPost(kCGSessionEventTap, keyDown);
        CGEventPost(kCGSessionEventTap, keyUp);
        
        CFRelease(keyDown);
        CFRelease(keyUp);
        
        log_info("Brightness increased");
    }
   
    void swipeLeft() {
        // Placeholder (trackpad gestures are harder)
    }

    void swipeRight() {
        // Placeholder
    }

    void performGestureAction(GestureType gesture) {
        switch (gesture) {
            case GestureType::THUMBS_UP:
                // click();
                increase_brightness();
                break;
            default:
                break;
        }
    }

}
