# Copyright 2019 The MediaPipe Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
	],
)

cc_binary(
	name = "main",
	deps = [
        "main_gpu",
        "//mediapipe/graphs/hand_tracking:multi_hand_mobile_calculators",
	],
)
