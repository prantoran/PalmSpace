#!/bin/bash

set -eof
# flag meanings: -e exit on error

GLOG_logtostderr=1 bazel-bin/desktop/main --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt --debug=1
