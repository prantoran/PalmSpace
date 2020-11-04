bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 desktop:main --verbose_failures
GLOG_logtostderr=1 bazel-bin/desktop/main --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt --debug=1







bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 mediapipe/examples/desktop/multi_hand_tracking:multi_hand_tracking_gpu

GLOG_logtostderr=1 bazel-bin/mediapipe/examples/desktop/multi_hand_tracking/multi_hand_tracking_gpu --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt




3x3

left hand - control window position

right hand index finger - trigger selection

Need a video - 
5 sec - moving the window with left hand

~10 sec - moving right hand index finger without any selection

~15 min - using right hand index finger to select cell 1-9








**WebSocket C++**

telnet host port












**Main GPU**

```
bbazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 desktop:main
```

```

GLOG_logtostderr=1 bazel-bin/desktop/main \
  --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt --debug=1 < data.in
  
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


** Creating WASM
```
emcc -O3 -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' \
  -s ALLOW_MEMORY_GROWTH=1 \
  -I . \
  webp.c \
  src/{dec,dsp,demux,enc,mux,utils}/*.c
  -o webp.html
```

```
em++ main.cc -s WASM=1 -o hello.html
em++ main.cc -s WASM=1 -o hello.html -02  # optimized

```


- run this where there is the html file: `python3 -m http.server 8080`




# Building with Bazel, Emscripten

```
bash javascript/scripts/build-wasm.sh && bash javascript/scripts/build-copy.sh

bash javascript/scripts/build-js.sh && bash javascript/scripts/build-copy.sh

```



loop
```
emscripten_set_main_loop(
  mainloop,  // function to call
  0,         // frame rate (0 = browser figures it out)
  1          // simulate infinite loop
);


mainloop {
        emscripten_cancel_main_loop();

}
```


If you do want to have full control over instantiating the WASM to reduce the bundle size, you may generate a pure WASM build by adding the link flag -s STANDALONE_WASM=1 inside the starlark file, hello-world/javascript/BUILD.


```
 bash javascript/scripts/build-wasm.sh && npm run build:copy",
```

width:640 height:480




```
bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 desktop:main --sandbox_debug --verbose_failures
```
```
GLOG_logtostderr=1 bazel-bin/desktop/main \
  --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt --debug=1 < data.in
```

```
bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 //javascript:twohand-wasm.js --config=wasm --sandbox_debug
```



need to enable RTTI
  does it solve?
    -DGOOGLE_PROTOBUF_NO_RTTI
    or
    #define GOOGLE_PROTOBUF_NO_RTTI
    https://groups.google.com/forum/#!topic/protobuf/d9G7B6lTkvs





GLOG_logtostderr=1 bazel-bin/desktop/main \
  --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt --debug=0 --output_video_path="/home/prantoran/work/src/github.com/prantoran/streamer/client/processed" --frame_width=400 --frame_height=400 < data.in



**adding emsdk as submodule**
git submodule add https://github.com/emscripten-core/emsdk submodules/emsdk
npm run submodule:update
npm run em:update
npm run em:init
