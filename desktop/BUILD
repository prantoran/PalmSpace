licenses(["notice"])  # Apache 2.0

exports_files(
    ["provisioning_profile.mobileprovision"],
    visibility = ["//visibility:public"],
)

package(default_visibility = [
    "//visibility:public",
])

# Linux only.
# Must have a GPU with EGL support:
# ex: sudo apt-get install mesa-common-dev libegl1-mesa-dev libgles2-mesa-dev
# (or similar nvidia/amd equivalent)


cc_library(
    name = "main_gpu",
    srcs = [
        "main.cc", 
        "handlers/handlers.h", "handlers/multihand_gpu.cc", 
        "handlers/anchor.cc",
        "handlers/trigger.cc",
        "handlers/initiator.cc",
        "anchors/anchors.h", "anchors/anchors.cc", 
        "anchors/anchors_dynamic.cc", "anchors/anchors_static.cc", 
        "triggers/triggers.h", "triggers/triggers.cc",
        "triggers/thumb.cc", "triggers/thumb_other.cc", 
        "triggers/pinch.cc", "triggers/wait.cc",
        "initiators/initiators.h", "initiators/initiators.cc",
        "initiators/initiators_default.cc",
        "initiators/initiators_twohand.cc",
        "initiators/area.cc",
    ],


    deps = [
        "@mediapipe//mediapipe/framework:calculator_framework",
        "@mediapipe//mediapipe/framework/formats:image_frame",
        "@mediapipe//mediapipe/framework/formats:image_frame_opencv",
        "@mediapipe//mediapipe/framework/port:commandlineflags",
        "@mediapipe//mediapipe/framework/port:file_helpers",
        "@mediapipe//mediapipe/framework/port:opencv_highgui",
        "@mediapipe//mediapipe/framework/port:opencv_imgproc",
        "@mediapipe//mediapipe/framework/port:opencv_video",
        "@mediapipe//mediapipe/framework/port:parse_text_proto",
        "@mediapipe//mediapipe/framework/port:status",
        "@mediapipe//mediapipe/gpu:gl_calculator_helper",
        "@mediapipe//mediapipe/gpu:gpu_buffer",
        "@mediapipe//mediapipe/gpu:gpu_shared_data_internal",
	    "@mediapipe//mediapipe/framework/formats:landmark_cc_proto",
        "@mediapipe//mediapipe/framework/formats:detection_cc_proto",
        "@mediapipe//mediapipe/framework/formats:rect_cc_proto",
	],
)

cc_binary(
	name = "main",
	deps = [
        "main_gpu",
        "@mediapipe//mediapipe/graphs/hand_tracking:multi_hand_mobile_calculators",
	],
)
