**Main GPU**

```
bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 \
  PalmSpace/examples/desktop:main
```

```

GLOG_logtostderr=1 bazel-bin/PalmSpace/examples/desktop/main \
  --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt
  
```

**TFlite Multi-Hand Tracking**

To build the multi-hand tracking demo using a TFLite model on desktop, use:

```
bazel build -c opt mediapipe/examples/desktop/multi_hand_tracking:multi_hand_tracking_tflite --define MEDIAPIPE_DISABLE_GPU=1
```

and run it using:

```
export GLOG_logtostderr=1

bazel-bin/mediapipe/examples/desktop/multi_hand_tracking/multi_hand_tracking_tflite \
  --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_desktop.pbtxt \
  --input_side_packets=input_video_path=/path/to/input/file,output_video_path=/path/to/output/file
```

To change the number of hands to `x` in this application, change:

1. `min_size:x` in `CollectionHasMinSizeCalculatorOptions` in `mediapipe/graphs/hand_tracking/multi_hand_tracking_desktop.pbtxt`.
2. `max_vec_size:x` in `ClipVectorSizeCalculatorOptions` in `mediapipe/examples/dekstop/hand_tracking/subgraphs/multi_hand_detection_cpu.pbtxt`.
