#include "gesture_engine.hpp"

using Result = mediapipe::tasks::vision::hand_landmarker::HandLandmarkerResult;

GestureType GestureEngine::detect(const Result& result) {
    if (isThumbsUp(result)) {
        log_info("Thumbs-up detected");
        return GestureType::THUMBS_UP;
    } else {
        log_info("No recognized gesture");
    }
    return GestureType::NONE;
}

bool GestureEngine::isThumbsUp(const Result& result) {
    if (result.hand_landmarks.empty()) return false;

    const auto& landmarks = result.hand_landmarks[0].landmarks;

    float thumb_tip_y = landmarks[4].y;
    float thumb_ip_y  = landmarks[3].y;

    float index_tip_y  = landmarks[8].y;
    float index_mcp_y  = landmarks[6].y;
    float middle_tip_y = landmarks[12].y;
    float middle_mcp_y = landmarks[10].y;
    float ring_tip_y   = landmarks[16].y;
    float ring_mcp_y   = landmarks[14].y;
    float pinky_tip_y  = landmarks[20].y;
    float pinky_mcp_y  = landmarks[18].y;

    bool thumb_extended = thumb_tip_y < thumb_ip_y - 0.05f;
    bool fingers_curled =
        index_tip_y  > index_mcp_y  - 0.05f &&
        middle_tip_y > middle_mcp_y - 0.05f &&
        ring_tip_y   > ring_mcp_y   - 0.05f &&
        pinky_tip_y  > pinky_mcp_y  - 0.05f;

    return thumb_extended && fingers_curled;
}
