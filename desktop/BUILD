
licenses(["notice"])  # Apache 2.0

package(default_visibility = [
    "//visibility:public",
])

load("@rules_foreign_cc//for_workspace:cmake_build.bzl", "cmake_tool")

load("@rules_foreign_cc//for_workspace:make_build.bzl", "make_tool")

toolchain(
    name = "built_cmake_toolchain",
    toolchain = "@rules_foreign_cc//tools/build_defs/native_tools:built_cmake",
    toolchain_type = "@rules_foreign_cc//tools/build_defs:cmake_toolchain",
)


load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
string_flag(name = "camera", build_setting_default = "default")


# Linux only.
# Must have a GPU with EGL support:
# ex: sudo apt-get install mesa-common-dev libegl1-mesa-dev libgles2-mesa-dev
# (or similar nvidia/amd equivalent)


config_setting(
    name="depth_camera",
    flag_values = {":camera": "realsense"},
)


cc_library(
    name = "camera_module",
    srcs = select({
        ":depth_camera": glob([
            "camera/*.h", "camera/*.cc",
        ]),
        "//conditions:default": [
            "camera/camera.h",
            "camera/camera.cc",
            "camera/opencv.cc",
        ],
    }),
    deps = select({
        ":depth_camera": [
            "//mediapipe/framework/port:opencv_highgui",
            "//mediapipe/framework/port:opencv_imgproc",
            "//mediapipe/framework/port:opencv_video",
            "//third_party:realsense",
        ],
        "//conditions:default": [
            "//mediapipe/framework/port:opencv_highgui",
            "//mediapipe/framework/port:opencv_imgproc",
            "//mediapipe/framework/port:opencv_video",
        ],
    }),
    defines = select({
        ":depth_camera": ['REALSENSE_CAM'],
        "//conditions:default": [],
    }),
    visibility = [ "//visibility:private" ],
)

cc_library(
    name = "main_gpu",
    srcs = glob([
        "handlers/*.h", "handlers/*.cc",
        "anchors/*.h", "anchors/*.cc",
        "triggers/*.h", "triggers/*.cc",
        "initiators/*.h", "initiators/*.cc",
        "config/*.h", "config/*.cc",
        "ui/*.h", "ui/*.cc",
        "userstudies/*.h", "userstudies/*.cc",
        "util/*.h", "util/*.cc",
    ]),
    deps = [
        "camera_module",
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/port:commandlineflags",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/framework/port:opencv_video",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/gpu:gpu_buffer",
        "//mediapipe/gpu:gpu_shared_data_internal",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/framework/formats:classification_cc_proto",
    ],
)


cc_binary(
	name = "main",
    srcs = ["main.cc"],
    # copts = ["-fno-rtti"],
	deps = [
        "main_gpu",
        "//mediapipe/graphs/hand_tracking:mobile_calculators",
    ],
)


DEFAULT_EMSCRIPTEN_LINKOPTS = [
    # "-flto",                        # Specify lto (has to be set on for compiler as well)
    "--bind",                       # Compiles the source code using the Embind bindings to connect C/C++ and JavaScript
    "--closure 1",                  # Run the closure compiler
    # "-s MALLOC=emmalloc",           # Switch to using the much smaller implementation
    "-s ALLOW_MEMORY_GROWTH=1",     # Our example doesn't need memory growth
    "-s USE_PTHREADS=0",            # Disable pthreads
    "-s ASSERTIONS=0",              # Turn off assertions
    "-s EXPORT_ES6=1",              # Export as es6 module, used for rollup
    "-s MODULARIZE=1",              # Allows us to manually invoke the initializatio of wasm
    "-s EXPORT_NAME=createModule",  # Not used, but good to specify
    # "-s USE_ES6_IMPORT_META=0",     # Disable loading from import meta since we use rollup
    "-s SINGLE_FILE=1"              # Pack all webassembly into base64
]

ASMJS_LINKOPTS = [
    "-s WASM=0",                    # Specify asm.js output
]

WASM_LINKOPTS = [
    "-s WASM=1",                    # Specify wasm output
]


cc_binary(
    name = "main_wasm",
    srcs = ["bindings/main.cpp"],
    linkopts = DEFAULT_EMSCRIPTEN_LINKOPTS + WASM_LINKOPTS,
    deps = [
        "main_gpu",
        "//mediapipe/graphs/hand_tracking:multi_hand_mobile_calculators",
    ],
)


cc_binary(
    name = "main_js",
    srcs = ["bindings/main.cpp"],
    linkopts = DEFAULT_EMSCRIPTEN_LINKOPTS + ASMJS_LINKOPTS,
    deps = [
        "main_gpu",
        "//mediapipe/graphs/hand_tracking:multi_hand_mobile_calculators",
    ],
)
