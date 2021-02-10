#!/bin/bash

set -eof
# flag meanings: -e exit on error

if [ ! -d "tmp" ]; then
    mkdir tmp
fi

# https://linuxize.com/post/bash-check-if-file-exists
if [ ! -f "tmp/WORKSPACE" ]; then
    cp WORKSPACE tmp/WORKSPACE
fi

EDEPTH=0

# https://archive.is/TRzn4
while getopts "d" opt; do 
    case $opt in 
        d)
            echo "-d (EDEPTH) triggered, Parameter $OPTARG" >&2
            EDEPTH=1
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit 1
            ;;
    esac
done
        

case `grep "linux_realsense" WORKSPACE >/dev/null; echo $?` in
  0)
    # code if found
    echo "linux_realsense defined in WORKSPACE"
    if [ "$EDEPTH" == "0" ]; then
        if [ ! -f "tmp/WORKSPACE_ndepth" ]; then
            head -n -11 WORKSPACE > tmp/WORKSPACE_ndepth
        fi
        rm WORKSPACE
        cp tmp/WORKSPACE_ndepth WORKSPACE
    fi
    ;;
  1)
    # code if not found
    echo "linux_realsense not in defined in WORKSPACE"
    if [ "$EDEPTH" == "1" ]; then
        rm WORKSPACE
        cp tmp/WORKSPACE WORKSPACE 
    fi
    
    ;;
  *)
    # code if an error occurred
    echo "error searching for linux_realsense in WORKSPACE"
    ;;
esac


OUTPUT_VIDEO_DIR="demo"

echo "OUTPUT_VIDEO_DIR="$OUTPUT_VIDEO_DIR

mkdir -p $OUTPUT_VIDEO_DIR


export camera=default # exporting makes it visible to scripts/build.sh
if [ "$EDEPTH" == "1" ]; then
    camera=realsense
fi

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
