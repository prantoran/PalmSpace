Copy the `PalmSpace` folder into the MediaPipe directory which has the `WORKSPACE` file.

Open a terminal in the MediaPipe directory which has the `WORKSPACE` file and execute the commands:

- build:
```
bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 \
  PalmSpace/examples/desktop:main
```

- run:
```
GLOG_logtostderr=1 bazel-bin/PalmSpace/examples/desktop/main \
  --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt --debug=1
```


** MediaPipe dependencies in WORKSPACE **
- libyuv
- org_tensorflow
  - patches
- ceres_solver
  - patches
- opencv



Updated build command:
```
bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 --experimental_repo_remote_exec\
  desktop:main
```