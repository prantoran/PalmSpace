
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.
#include <cstdlib>

#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/port/status.h"

#include "desktop/anchors/anchors.h"
#include "desktop/triggers/triggers.h"
#include "desktop/initiators/initiators.h"

#include "desktop/handlers/handlers.h";

#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>

#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"


#include <cstdlib>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

DEFINE_string(
    calculator_graph_config_file, 
    "mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt",
    "Name of file containing text format CalculatorGraphConfig proto.");
DEFINE_string(input_video_path, "",
              "Full path of video to load. "
              "If not provided, attempt to use a webcam.");
DEFINE_string(output_video_path, "",
              "Full path of where to save result (.mp4 only). "
              "If not provided, show result in a window.");

DEFINE_int32(frame_width, 640, "frame/screen width in pixels.");
DEFINE_int32(frame_height, 480, "frame/screen height in pixels.");
DEFINE_int32(fps, 30, "frames per second.");
DEFINE_int32(debug, 0, "debug mode");

std::shared_ptr<MediaPipeMultiHandGPU> mp_graph = NULL;


int main(int argc, char** argv) {

  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::cout << "frame_width:" << FLAGS_frame_width << " frame_height:" << FLAGS_frame_height << "\n";
  
  int choice_anchor;
  int choice_trigger;
  int choice_initiator;
  int choice_divisions;
  
  if (mp_graph == NULL) {
    mp_graph = std::make_shared<MediaPipeMultiHandGPU>();
    
    try {
      auto tst = cv::Scalar(25,25,25);
    } catch (const std::exception& e) {
      std::cout << "exception tst:" << e.what() << "\n";
    } 

    AnchorHandler handler_anchor;
    handler_anchor._dynamic = AnchorDynamic(cv::Scalar(25, 25, 255), cv::Scalar(255, 25, 25));


    // note: if image path invalid then cv::resize() error occurs
    handler_anchor._static = AnchorStatic(
                              cv::Scalar(25, 25, 255), 
                              cv::Scalar(255, 25, 25), 
                              "/home/prantoran/work/src/github.com/google/mediapipe/desktop/anchors/Hand.png");



    TriggerHandler handler_trigger;
    handler_trigger._thumb = TriggerThumb(FLAGS_frame_width, FLAGS_frame_height);
    handler_trigger._thumb_other = TriggerThumbOther(FLAGS_frame_width, FLAGS_frame_height);
    handler_trigger._pinch = TriggerPinch(FLAGS_frame_width, FLAGS_frame_height);
    handler_trigger._wait = TriggerWait(FLAGS_frame_width, FLAGS_frame_height, -1);
    // handler_trigger._tap = TriggerTap(FLAGS_frame_width, FLAGS_frame_height);
    handler_trigger._dwell = TriggerDwell();


    InitiatorHandler handler_initiator;
    handler_initiator._default = InitiatorDefault();
    handler_initiator._twohand = InitiatorTwoHand();



    mp_graph->anchor = handler_anchor;
    mp_graph->initiator = handler_initiator;
    mp_graph->trigger = handler_trigger;
  }

  // choice_initiator = 1;
  // choice_anchor = 1;
  // choice_trigger = 1;

  // while(true) {

    std::cout << "Pick initiator (1 : Default, 2: Two-Hand): ";
    std::cin >> choice_initiator;
    mp_graph->initiator._choice = choice_initiator;

    std::cout << "Pick anchor (1 : Dynamic, 2: static, 3: midair): ";
    std::cin >> choice_anchor;
    mp_graph->anchor._choice = choice_anchor;

    std::cout << "Pick the number of cells per row/col (3 - 10): ";
    std::cin>> choice_divisions;
    mp_graph->anchor.setDivisions(choice_divisions);
    
    std::cout << "Pick trigger (1: Thumb of base palm, 2: Thumb of free palm, 3: Pinch with free palm, 4: Wait to select, 5: Tap, 6: Dwell): ";
    std::cin >> choice_trigger;
    mp_graph->trigger._choice = choice_trigger;
    mp_graph->trigger._wait.choice = choice_anchor;

    if (choice_anchor == 1 && choice_initiator == 2) {
      exit(-1);
    }

    std::cout << "setting up strict: whether or not to look for index pointer from 2nd hand only\n";
    if (choice_trigger == 1 || choice_trigger == 5) {
      mp_graph->initiator.setStrict(true); // only look for pointer in second hand
    } else if (choice_trigger == 6) {
      std::cout << "given dwell trigger, checking choice_anchor\n";
      if (choice_anchor != 3) {
        mp_graph->initiator.setStrict(true); // only look for pointer in second hand
      }
    }

    ::mediapipe::Status run_status = mp_graph->run(
        FLAGS_calculator_graph_config_file, 
        FLAGS_input_video_path, 
        FLAGS_output_video_path, 
        FLAGS_frame_width, 
        FLAGS_frame_height, 
        FLAGS_fps,
        FLAGS_debug); //,
        // FLAGS_image_ext,
        // FLAGS_read_delay,
        // inq, outq);
    
    if (!run_status.ok()) {
      LOG(ERROR) << "Failed to run the graph: " << run_status.message();
      // return EXIT_FAILURE;
    } else {
      LOG(INFO) << "Success!";
    }
  // }

  // std::this_thread::sleep_for(std::chrono::microseconds(20sssss0000));


  return EXIT_SUCCESS;
}

