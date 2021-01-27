#!/bin/bash

set -eof
# flag meanings: -e exit on error

OUTPUT_VIDEO_DIR="demo"

echo "OUTPUT_VIDEO_DIR="$OUTPUT_VIDEO_DIR

mkdir -p $OUTPUT_VIDEO_DIR

./scripts/build.sh

# sudo udevadm control --reload-rules && udevadm trigger


# sudo modprobe -r uvcvideo
# sudo modprobe uvcvideo


# GLOG_logtostderr=1 bazel-bin/desktop/main --dev_video=3 --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt --debug=0 < scripts/in
GLOG_logtostderr=1 bazel-bin/desktop/main \
    --calculator_graph_config_file=mediapipe/graphs/hand_tracking/hand_tracking_desktop_live_gpu.pbtxt \
    --output_video_path=$OUTPUT_VIDEO_DIR \
    --debug=0 \
    < scripts/in
