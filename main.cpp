
#include <ApplicationServices/ApplicationServices.h>

#include <atomic>
#include <csignal>
#include <unistd.h>
#include <os/log.h>

#include <opencv2/opencv.hpp>

#include "mediapipe/tasks/cc/vision/hand_landmarker/hand_landmarker.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"

#include "tools/cpp/runfiles/runfiles.h"

#include "utils/log.hpp"
#include "core/gesture_engine.hpp"
#include "platform/macos_bridge.h"
#include "utils/config.h"

std::atomic<bool> running(true);

void handle_signal(int) {
    running = false;
}

void setup_signal_handlers() {
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);
}

void setup_logging() {
    os_log(OS_LOG_DEFAULT, "Gesture agent started");

    // custom logging 
    init_logging();
    log_info("Gesture agent started");

    globalTimer.Reset(); 
}

inline void cleanup_logging() {
    os_log(OS_LOG_DEFAULT, "Gesture agent shutting down");

    // custom logging
    log_info("Gesture agent shutting down");
    
    globalTimer.End("Total runtime");
}

std::unique_ptr<bazel::tools::cpp::runfiles::Runfiles> 
setup_bazel_runfiles(const char* argv0) {
    using bazel::tools::cpp::runfiles::Runfiles;
    std::string error;
    std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv0, &error));
    if (!runfiles) {
        log_error("Failed to create Bazel runfiles: " + error);
        return nullptr;
    }
    log_info("Bazel runfiles setup successful");
    return runfiles;
}

std::unique_ptr<mediapipe::tasks::vision::hand_landmarker::HandLandmarker>
setup_hand_landmarker(bazel::tools::cpp::runfiles::Runfiles* runfiles) {
    std::string model_path = runfiles->Rlocation(mediapipeConfigs::kHandLandmarkerModelPath);
    auto options = std::make_unique<mediapipe::tasks::vision::hand_landmarker::HandLandmarkerOptions>();
    options->base_options.model_asset_path = model_path;
    options->running_mode = mediapipe::tasks::vision::core::RunningMode::VIDEO;
    options->num_hands = mediapipeConfigs::kNumHands;
    options->min_hand_detection_confidence = mediapipeConfigs::kMinHandDetectionConfidence;
    options->min_hand_presence_confidence = mediapipeConfigs::kMinHandPresenceConfidence;
    options->min_tracking_confidence = mediapipeConfigs::kMinTrackingConfidence;

    auto hand_landmarker_or_status = mediapipe::tasks::vision::hand_landmarker::HandLandmarker::Create(std::move(options));
    if (!hand_landmarker_or_status.ok()) {
        log_error("Failed to create hand landmarker: " + hand_landmarker_or_status.status().ToString());
        return nullptr;
    }
    log_info("Hand landmarker created");
    return std::move(hand_landmarker_or_status.value());
}

int main(int argc, char** argv) {

    setup_signal_handlers();
    setup_logging();

    auto runfiles_ptr = setup_bazel_runfiles(argv[0]);
    if (!runfiles_ptr) return -1;
    auto landmarker_ptr = setup_hand_landmarker(runfiles_ptr.get());

    // Open webcam
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) { log_error("Could not open video camera"); return -1; }

    // Gesture engine setup
    GestureEngine gestureEngine;
    GestureType lastGesture = GestureType::NONE;
    int stableFrames = gestureEngineConfigs::kStableFrames;
    constexpr int kTriggerFrames = gestureEngineConfigs::kTriggerFrames; // debounce threshold
    int64_t frame_timestamp_ms = gestureEngineConfigs::kframeTimestampMs;

    while (running) {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            usleep(33333);
            continue;
        }

        // Flip for selfie-view gestures
        cv::flip(frame, frame, 1);

        // Convert BGR → RGB
        cv::Mat frame_rgb;
        cv::cvtColor(frame, frame_rgb, cv::COLOR_BGR2RGB);

        // Wrap OpenCV frame for MediaPipe
        auto image_frame = std::make_shared<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGB,
            frame_rgb.cols,
            frame_rgb.rows,
            frame_rgb.step,
            frame_rgb.data,
            [](uint8_t*) {} // OpenCV owns the memory
        );

        mediapipe::Image mp_image(image_frame);

        // Run hand tracking
        auto result =
            (landmarker_ptr)->DetectForVideo(mp_image, frame_timestamp_ms);

        if (result.ok()) {
            GestureType gesture =
                gestureEngine.detect(*result);

            if (gesture == lastGesture && gesture != GestureType::NONE) {
                stableFrames++;

                if (stableFrames == kTriggerFrames) {
                    log_info("Gesture confirmed");
                    macos::performGestureAction(gesture);
                }
            } else {
                stableFrames = 0;
            }

            lastGesture = gesture;
        } else {
            log_error("Hand detection failed: " +
                      result.status().ToString());
        }

        frame_timestamp_ms += 33; // ~30 FPS
        usleep(33333);
    }

    cleanup_logging();

    return 0;
}
