
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

#include "log/log.hpp"

using bazel::tools::cpp::runfiles::Runfiles; 

std::atomic<bool> running(true);

void handle_signal(int) {
    running = false;
}

// Detect thumbs up gesture from hand landmarks
bool is_thumbs_up(const mediapipe::tasks::vision::hand_landmarker::HandLandmarkerResult& result) {
    if (result.hand_landmarks.empty()) {
        return false;
    }
    
    // Get the first detected hand
    const auto& landmarks = result.hand_landmarks[0].landmarks;

    // Landmark indices (MediaPipe hand landmark model):
    // 0 = wrist
    // 4 = thumb tip
    // 3 = thumb IP joint
    // 2 = thumb MCP joint
    // 8 = index finger tip
    // 6 = index finger MCP joint
    // 12 = middle finger tip
    // 10 = middle finger MCP joint
    // 16 = ring finger tip
    // 14 = ring finger MCP joint
    // 20 = pinky tip
    // 18 = pinky MCP joint
    
    // Thumbs up criteria:
    // 1. Thumb is extended (tip is higher than IP joint in y-axis)
    // 2. Other four fingers are curled (tips below their MCP joints)
    
    float thumb_tip_y = landmarks[4].y;
    float thumb_ip_y = landmarks[3].y;
    
    float index_tip_y = landmarks[8].y;
    float index_mcp_y = landmarks[6].y;
    
    float middle_tip_y = landmarks[12].y;
    float middle_mcp_y = landmarks[10].y;
    
    float ring_tip_y = landmarks[16].y;
    float ring_mcp_y = landmarks[14].y;
    
    float pinky_tip_y = landmarks[20].y;
    float pinky_mcp_y = landmarks[18].y;
    
    // Check thumb is extended upward (lower y value = higher on screen)
    bool thumb_extended = thumb_tip_y < thumb_ip_y - 0.05f;
    
    // Check other fingers are curled (tips should be below/similar to MCP joints)
    bool index_curled = index_tip_y > index_mcp_y - 0.05f;
    bool middle_curled = middle_tip_y > middle_mcp_y - 0.05f;
    bool ring_curled = ring_tip_y > ring_mcp_y - 0.05f;
    bool pinky_curled = pinky_tip_y > pinky_mcp_y - 0.05f;
    
    return thumb_extended && index_curled && middle_curled && ring_curled && pinky_curled;
}

int main(int argc, char** argv) {    
    init_logging();

    // Register signal handlers so launchd can stop us cleanly
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    os_log(OS_LOG_DEFAULT, "Gesture agent started");
    log_info("Gesture agent started");
    globalTimer.Reset();

    std::string error;
    std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0], &error));
    if (runfiles == nullptr) {
        log_error("Failed to create runfiles: " + error);
        return -1;
    }

    std::string model_path = runfiles->Rlocation("mediapipe/mediapipe/examples/desktop/gesture_agent/hand_landmarker.task");
    log_info("Model path: " + model_path);

    // Open video capture (webcam)
    cv::VideoCapture cap(0); // Open default camera
    if (!cap.isOpened()) {
        log_error("Could not open video camera!");
        return -1;
    }

    // Initialize MediaPipe Hand Landmarker
    auto options = std::make_unique<mediapipe::tasks::vision::hand_landmarker::HandLandmarkerOptions>();
    options->base_options.model_asset_path = model_path;
    
    options->running_mode = mediapipe::tasks::vision::core::RunningMode::VIDEO;
    options->num_hands = 1;
    options->min_hand_detection_confidence = 0.5;
    options->min_hand_presence_confidence = 0.5;
    options->min_tracking_confidence = 0.5;

    auto landmarker = mediapipe::tasks::vision::hand_landmarker::HandLandmarker::Create(std::move(options));
    if (!landmarker.ok()) {
        log_error("Failed to create hand landmarker: " + landmarker.status().ToString());
        return -1;
    } else {
        log_info("Hand landmarker created successfully.");
    }

    int64_t frame_timestamp_ms = 0;

    while (running) {
        cv::Mat frame;
        cap >> frame; // Capture a single frame

        if (!frame.empty()) {
            // 1. Flip the frame (selfie view is more intuitive for gestures)
            cv::flip(frame, frame, 1);

            // 2. Convert to RGB (OpenCV uses BGR, MediaPipe needs RGB)
            cv::Mat frame_rgb;
            cv::cvtColor(frame, frame_rgb, cv::COLOR_BGR2RGB);
            
            // log_info("Camera frame captured successfully. Size: " + 
            //         std::to_string(frame.cols) + "x" + std::to_string(frame.rows));
            
            // 3. Convert to MediaPipe Image
            auto image_frame = std::make_shared<mediapipe::ImageFrame>(
                mediapipe::ImageFormat::SRGB,
                frame_rgb.cols,
                frame_rgb.rows,
                frame_rgb.step,
                frame_rgb.data,
                [](uint8_t*){} // No-op deleter since OpenCV owns the data
            );
            
            mediapipe::Image mp_image(image_frame);
            
            // 4. Process with hand landmarker
            auto result = (*landmarker)->DetectForVideo(mp_image, frame_timestamp_ms);
            
            if (result.ok()) {
                // 5. Check for thumbs up gesture
                if (is_thumbs_up(*result)) {
                    log_info("👍 THUMBS UP DETECTED!");
                    os_log(OS_LOG_DEFAULT, "Thumbs up gesture detected");
                } else {
                    log_info("No thumbs up detected in this frame.");
                }
            } else {
                log_error("Hand detection failed: " + result.status().ToString());
            }
            
            frame_timestamp_ms += 33; // Increment by ~33ms for 30 FPS
        }
        
        // We don't want to show a window (cv::imshow) because this is a background daemon!
        // We just want to process the data.
        usleep(33333); // Run at ~30 FPS
    }

    globalTimer.End("Total runtime");

    os_log(OS_LOG_DEFAULT, "Gesture agent shutting down");
    log_info("Gesture agent shutting down");
    
    return 0;
}