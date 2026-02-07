#pragma once

#include "mediapipe/tasks/cc/vision/hand_landmarker/hand_landmarker.h"

#include "gesture_types.h"
#include "utils/log.hpp"

class GestureEngine {
public:
    GestureType detect(const mediapipe::tasks::vision::hand_landmarker::HandLandmarkerResult& result);

private:
    bool isThumbsUp(const mediapipe::tasks::vision::hand_landmarker::HandLandmarkerResult& result);
};
