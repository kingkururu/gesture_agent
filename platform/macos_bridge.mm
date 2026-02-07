#import "macos_bridge.h"

namespace macos {

    void click() {
        CGPoint pos = CGEventGetLocation(CGEventCreate(NULL));

        CGEventRef down = CGEventCreateMouseEvent( NULL, kCGEventLeftMouseDown, pos, kCGMouseButtonLeft );
        CGEventRef up = CGEventCreateMouseEvent( NULL, kCGEventLeftMouseUp, pos, kCGMouseButtonLeft );

        CGEventPost(kCGHIDEventTap, down);
        CGEventPost(kCGHIDEventTap, up);

        CFRelease(down);
        CFRelease(up);

        // int posX = (int)pos.x;
        // int posY = (int)pos.y;

        // log_info(
        //     std::string("Simulated mouse click at ")
        //     + std::to_string(posX)
        //     + ", "
        //     + std::to_string(posY)
        // );      
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
                click();
                break;
            default:
                break;
        }
    }

}
