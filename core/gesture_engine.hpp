#pragma once

#include "gesture_types.h"
#include "mediapipe/tasks/cc/vision/hand_landmarker/hand_landmarker.h"

class GestureEngine {
public:
    GestureType detect(
        const mediapipe::tasks::vision::hand_landmarker::HandLandmarkerResult& result
    );

private:
    bool isThumbsUp(
        const mediapipe::tasks::vision::hand_landmarker::HandLandmarkerResult& result
    );
};
