
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

// #define CVUI_DISABLE_COMPILATION_NOTICES // to turn off debug msgs from cvui
#define CVUI_IMPLEMENTATION
#include "desktop/ui/cvui.h"
#include "desktop/ui/ui.h"

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



  int choice_anchor = 1;
  int choice_trigger = 6;
  int choice_initiator = 1;
  int choice_divisions = 6;
  

  
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

  
  cvui::init(WINDOW_NAME);
    // Create a frame
  cv::Mat frame = cv::Mat(cv::Size(FLAGS_frame_width, FLAGS_frame_height), CV_8UC3);
	bool use_canny = false;
	int low_threshold = 50, high_threshold = 150;
  int cellcnt = choice_divisions;

  bool onehand = false, twohand = false;
  bool ancdyn = false, ancstat  = false, ancmid  = false; 
  bool trigpalmbase = false, trigpalmfree = false, trigpinch = false, trigwait = false, trigtap = false, trigdwell = false;

  int scalex = -10 + FLAGS_frame_width/4, scaley = -10 + FLAGS_frame_height/4;


  std::string errormsg = "";
  bool valid = true;

  bool debug = FLAGS_debug;

  while(true) {
    // std::cerr << "rendering ui\n";
        // clear the frame
    frame = cv::Scalar(79,79,79);
        // render a message in the frame at position (10, 15)

    cvui::checkbox(frame, FLAGS_frame_width - 120, FLAGS_frame_height - 100, "Debug", &debug);
    
    cvui::window(frame, scalex + 10, scaley + 10, 100, 80, "Initiator");
		cvui::checkbox(frame, scalex + 15, scaley + 30, "One Hand", &onehand);
    cvui::checkbox(frame, scalex + 15, scaley + 50, "Two Hand", &twohand);

    
    cvui::window(frame, scalex + 10, scaley + 100, 100, 120, "Anchor");
		cvui::checkbox(frame, scalex + 15, scaley + 130, "Dynamic", &ancdyn);
    cvui::checkbox(frame, scalex + 15, scaley + 150, "Static", &ancstat);
    cvui::checkbox(frame, scalex + 15, scaley + 170, "MidAir", &ancmid);


    cvui::window(frame, scalex + 120, scaley + 10, 200, 150, "Trigger");
		cvui::checkbox(frame, scalex + 125, scaley + 30, "Thumb of base palm", &trigpalmbase);
    cvui::checkbox(frame, scalex + 125, scaley + 50, "Thumb of free palm", &trigpalmfree);
    cvui::checkbox(frame, scalex + 125, scaley + 70, "Pinch with free palm", &trigpinch);
    cvui::checkbox(frame, scalex + 125, scaley + 90, "Wait to select", &trigwait);
    cvui::checkbox(frame, scalex + 125, scaley + 110, "Tap", &trigtap);
    cvui::checkbox(frame, scalex + 125, scaley + 130, "Dwell", &trigdwell);

    if (cellcnt < 3) cellcnt = 3;
    if (cellcnt > 10) cellcnt = 10;
    cvui::window(frame, scalex + 120, scaley + 170, 200, 50, "Number of cells per row/col");
    cvui::counter(frame, scalex + 175, scaley + 195, &cellcnt);

    if (cvui::button(frame, FLAGS_frame_width - 120, FLAGS_frame_height - 50, 100, 30, "Next")) {
      int cnt = 0;
      valid = true;
      if (onehand) cnt ++;
      if (twohand) cnt ++;
      if (cnt != 1) {
        valid = 0;
        errormsg = "Select exactly 1 initiator.";
      }

      if (valid) {
        cnt = 0;
        if (ancdyn) cnt ++;
        if (ancstat) cnt ++;
        if (ancmid) cnt ++;
        if (cnt != 1) {
          valid = 0;
          errormsg = "Select exactly 1 anchor.";
        }
      }

      if (valid) {
        cnt = 0;
        if (trigpalmbase) cnt ++;
        if (trigpalmfree) cnt ++;
        if (trigpinch) cnt ++;
        if (trigwait) cnt ++;
        if (trigtap) cnt ++;
        if (trigdwell) cnt ++;
        if (cnt != 1) {
          valid = 0;
          errormsg = "Select exactly 1 trigger.";
        }
      }



      if (valid) {
        break;
      }
    }


    if (!valid) {
      cvui::text(frame, 10, FLAGS_frame_height-50, errormsg, 0.5, 0xFEFEFE);
    }
        // Show window content
    cvui::imshow(WINDOW_NAME, frame);

    // your app logic here
    if (cv::waitKey(20) == 27) {
      break;
    }
  }

  if (onehand) choice_initiator = 1;
  if (twohand) choice_initiator = 2;

  if (ancdyn) choice_anchor = 1;
  if (ancstat) choice_anchor = 2;
  if (ancmid) choice_anchor = 3;

  if (trigpalmbase) choice_trigger = 1;
  if (trigpalmfree) choice_trigger = 2;
  if (trigpinch) choice_trigger = 3;
  if (trigwait) choice_trigger = 4;
  if (trigtap) choice_trigger = 5;
  if (trigdwell) choice_trigger = 6;

  choice_divisions = cellcnt;

  // std::cout << "Pick initiator (1 : Default, 2: Two-Hand): ";
  // std::cin >> choice_initiator;

  // std::cout << "Pick anchor (1 : Dynamic, 2: static, 3: midair): ";
  // std::cin >> choice_anchor;

  // std::cout << "Pick the number of cells per row/col (3 - 10): ";
  // std::cin>> choice_divisions;
  
  // std::cout << "Pick trigger (1: Thumb of base palm, 2: Thumb of free palm, 3: Pinch with free palm, 4: Wait to select, 5: Tap, 6: Dwell): ";
  // std::cin >> choice_trigger;

  mp_graph->initiator._choice = choice_initiator;
  mp_graph->anchor._choice = choice_anchor;
  mp_graph->anchor.setDivisions(choice_divisions);
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
      std::cerr << "setting initiator strict\n";
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
      debug); 
  
  if (!run_status.ok()) {
    LOG(ERROR) << "Failed to run the graph: " << run_status.message();
    // return EXIT_FAILURE;
  } else {
    LOG(INFO) << "Success!";
  }
  // }

  return EXIT_SUCCESS;
}

