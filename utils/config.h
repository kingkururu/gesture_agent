#pragma once

#include <string>

namespace mediapipeConfigs {
    inline constexpr const char* kHandLandmarkerModelPath = "mediapipe/mediapipe/examples/desktop/gesture_agent/hand_landmarker.task";
    inline constexpr char kNumHands = 1;
    inline constexpr float kMinHandDetectionConfidence = 0.5f;
    inline constexpr float kMinHandPresenceConfidence = 0.5f;
    inline constexpr float kMinTrackingConfidence = 0.5f;
}

namespace gestureEngineConfigs {
    inline constexpr float kTriggerFrames = 5;
}






// Gesture thresholds

// distance deltas (e.g. how far a swipe must move)

// angle tolerances

// pinch distance

// Timing values

// debounce frames

// cooldown durations

// minimum gesture duration

// Camera / processing assumptions

// target FPS

// frame interval

// Sensitivity constants

// how “strict” a thumbs-up is

// Debug toggles

// enable gesture logging

// enable visual overlay