
licenses(["notice"])  # Apache 2.0

package(default_visibility = [
    "//visibility:public",
])


# Linux only.
# Must have a GPU with EGL support:
# ex: sudo apt-get install mesa-common-dev libegl1-mesa-dev libgles2-mesa-dev
# (or similar nvidia/amd equivalent)


cc_library(
    name = "main_gpu",
    srcs = glob([
        # "main.cc",
        "handlers/*.h", "handlers/*.cc",
        "anchors/*.h", "anchors/*.cc",
        "triggers/*.h", "triggers/*.cc",
        "initiators/*.h", "initiators/*.cc",
        "config/*.h", "config/*.cc",
        "ui/*.h", "ui/*.cc",
    ]),

    deps = [
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
        "//mediapipe/framework/formats:detection_cc_proto",
        "//mediapipe/framework/formats:rect_cc_proto",
        # "@taxi_code_socialworkout_webrtc_linux//:rtc",
	],
)

cc_binary(
	name = "main",
    srcs = ["main.cc"],
    # copts = ["-fno-rtti"],
	deps = [
        "main_gpu",
        "//mediapipe/graphs/hand_tracking:multi_hand_mobile_calculators",
        # "@taxi_code_socialworkout_webrtc_linux//:rtc",
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
