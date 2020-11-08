#!/bin/bash

set -eof
# flag meanings: -e exit on error

./scripts/build.sh


GLOG_logtostderr=1 bazel-bin/desktop/main --dev_video=3 --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt --debug=0 < scripts/in
