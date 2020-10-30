
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.

// stl
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <unordered_map>

#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/port/status.h"

// opencv
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

#include "desktop/anchors/anchors.h"
#include "desktop/triggers/triggers.h"
#include "desktop/initiators/initiators.h"
#include "desktop/handlers/handlers.h";
#include "desktop/ui/menu.h"
#include "desktop/config/config.h";

#include "desktop/config/choices.h";


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
DEFINE_int32(dev_video, 0, "/dev/video*");

std::shared_ptr<MediaPipeMultiHandGPU> mp_graph = NULL;


void checkOpenCVHardwareSupport() {
  // CV_CPU_SSE4.1, CV_CPU_SSE4.2 not defined OpenCV v3.4
  int features [] = {CV_CPU_MMX, 
    CV_CPU_SSE, CV_CPU_SSE2, CV_CPU_SSE3, 
    CV_CPU_POPCNT, CV_CPU_AVX, CV_CPU_AVX2};

  std::unordered_map<int, std::string> names = {
    {CV_CPU_MMX, "CV_CPU_MMX"}, 
    {CV_CPU_SSE, "CV_CPU_SSE"},
    {CV_CPU_SSE2, "CV_CPU_SSE2"},
    {CV_CPU_SSE3, "CV_CPU_SSE3"},
    {CV_CPU_POPCNT, "CV_CPU_POPCNT"},
    {CV_CPU_AVX, "CV_CPU_AVX"},
    {CV_CPU_AVX2, "CV_CPU_AVX2"}
  };

  for (const auto& f: features) {
    std::cout << f << ". " << names[f] << " " << (cv::checkHardwareSupport(f)? "enabled": "disabled") << "\n";
  }

  std::cout << "Number of logical CPUs available to OpenCV: " << cv::getNumberOfCPUs() << "\n";
  std::cout << "Number of threads available to OpenCV: " << cv::getNumThreads() << "\n";
  std::cout << "OpenCV is optimized code enabled: " << (cv::useOptimized()? "yes": "no") << "\n";
}


Choices choices;

int main(int argc, char** argv) {

  checkOpenCVHardwareSupport();

  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::cout << "frame_width:" << FLAGS_frame_width << " frame_height:" << FLAGS_frame_height << "\n";


  const bool load_video = !FLAGS_input_video_path.empty();
  const bool save_video = !FLAGS_output_video_path.empty();

  int choice_anchor = 1;
  int choice_trigger = 6;
  int choice_initiator = 1;
  int choice_divisions = 6;
  int choice_screensize = 1;
  int choice_debug = FLAGS_debug;

  
  
  if (mp_graph == NULL) {
    mp_graph = std::make_shared<MediaPipeMultiHandGPU>(APP_NAME);
    
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
    handler_trigger._tap_depth_area = TriggerTapDepthArea(
      load_video, 
      save_video, 
      FLAGS_dev_video, 
      FLAGS_fps,
      FLAGS_frame_width, 
      FLAGS_frame_height);


    InitiatorHandler handler_initiator;
    handler_initiator._default = InitiatorDefault();
    handler_initiator._twohand = InitiatorTwoHand();


    mp_graph->anchor = handler_anchor;
    mp_graph->initiator = handler_initiator;
    mp_graph->trigger = handler_trigger;
  }

  PalmSpaceUI::Menu menu = PalmSpaceUI::Menu(
    FLAGS_frame_width,
    FLAGS_frame_height,
    choice_divisions,
    choice_screensize,
    choice_debug,
    APP_NAME);

  menu.run();

  menu.get_choices(
    choice_initiator,
    choice_anchor,
    choice_trigger,
    choice_divisions,
    choice_screensize,
    choice_debug);

  mp_graph->initiator._choice = choice_initiator;
  mp_graph->anchor._choice = choice_anchor;
  mp_graph->anchor.setDivisions(choice_divisions);
  mp_graph->trigger._choice = choice_trigger;
  mp_graph->trigger._wait.choice = choice_anchor;
  
  eScreenSize ssize = choices.getScreenSize(choice_screensize);
  
  mp_graph->anchor.setScreenSize(ssize);
    

  if (choice_anchor == 1 && choice_initiator == 2) {
    exit(-1);
  }

  std::cout << "setting up strict: whether or not to look for index pointer from 2nd hand only\n";
  if (choice_trigger == 1 || choice_trigger == 5) {
    mp_graph->initiator.setStrict(true); // only look for pointer in second hand
  } else if (choice_trigger == 6) {
    std::cout << "given dwell trigger, checking choice_anchor\n";
    if (choice_anchor != 3) {      mp_graph->initiator.setStrict(true); // only look for pointer in second hand
    }
  }

  ::mediapipe::Status run_status = mp_graph->run(
      FLAGS_calculator_graph_config_file, 
      FLAGS_input_video_path, 
      FLAGS_output_video_path, 
      FLAGS_frame_width, 
      FLAGS_frame_height, 
      FLAGS_fps,
      choice_debug,
      FLAGS_dev_video,
      load_video,
      save_video); 
  
  if (!run_status.ok()) {
    LOG(ERROR) << "Failed to run the graph: " << run_status.message();
    // return EXIT_FAILURE; // probably defined in MediaPipe:ports
  } else {
    LOG(INFO) << "Success!";
  }
  // }

  return EXIT_SUCCESS; // probably defined in MediaPipe:ports
}

