cc_library(
    name = "utils",
    hdrs = ["utils/log.hpp", "utils/config.h"],
    srcs = ["utils/log.cpp"],
    includes = ["."],
    deps = ["@com_github_gabime_spdlog//:spdlog", 
            "@bazel_tools//tools/cpp/runfiles",  
    ],
)

cc_library(
    name = "gesture_core",
    hdrs = [
        "core/gesture_types.h",
        "core/gesture_engine.hpp",
        "utils/log.hpp",      
        "utils/config.h",  
    ],
    srcs = [
        "core/gesture_engine.cpp",
        "utils/log.cpp", 
    ],
    includes = ["."],
    deps = [
        ":utils",
        "//mediapipe/tasks/cc/vision/hand_landmarker:hand_landmarker",
    ],
)

objc_library(
    name = "macos_bridge",
    srcs = ["platform/macos_bridge.mm"],
    hdrs = ["platform/macos_bridge.h"],
    sdk_frameworks = [
        "CoreGraphics",
        "CoreFoundation",
        "ApplicationServices",
    ],
    deps = [
        ":gesture_core",
    ],
)

cc_binary(
    name = "gesture_run",
    srcs = [
        "main.cpp",
        "core/gesture_engine.cpp",
        "core/gesture_engine.hpp",
        "core/gesture_types.h",
        "platform/macos_bridge.h",  
        "utils/log.cpp",
        "utils/log.hpp",
        "utils/config.h",
    ],
    includes = ["."],
    data = ["hand_landmarker.task"],
    deps = [
        ":macos_bridge", 
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