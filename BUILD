cc_binary(
    name = "gesture_run",
    srcs = [
        "main.cpp",
        "log/log.cpp",
        "log/log.hpp",
    ],
    includes = ["."],
    data = ["hand_landmarker.task"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/tasks/cc/vision/hand_landmarker:hand_landmarker",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "@macos_opencv//:opencv",
        "@bazel_tools//tools/cpp/runfiles",
        "@com_github_gabime_spdlog//:spdlog",
    ],
    linkopts = [
        "-framework CoreGraphics",
        "-framework CoreFoundation",
        "-framework ApplicationServices",
        "-framework Metal",
        "-framework MetalKit",
        "-framework CoreVideo",
        "-framework AVFoundation",
    ],
)