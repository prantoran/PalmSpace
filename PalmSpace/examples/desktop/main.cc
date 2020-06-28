
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.
#include <cstdlib>

#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/port/status.h"

#include "anchors/anchors.h"
#include "triggers/triggers.h"
#include "initiators/initiators.h"


#include "handlers/handlers.h";

DEFINE_string(
    calculator_graph_config_file, "",
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


int main(int argc, char** argv) {

  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::cerr << "frame_width:" << FLAGS_frame_width << " frame_height:" << FLAGS_frame_height << "\n";

  AnchorHandler handler_anchor;
  handler_anchor._dynamic = AnchorDynamic(cv::Scalar(25, 25, 255), cv::Scalar(255, 25, 25));
  handler_anchor._static = AnchorStatic(cv::Scalar(25, 25, 255), cv::Scalar(255, 25, 25));
  int choice_anchor;

  TriggerHandler handler_trigger;
  handler_trigger._thumb = TriggerThumb(FLAGS_frame_width, FLAGS_frame_height);
  handler_trigger._thumb_other = TriggerThumbOther(FLAGS_frame_width, FLAGS_frame_height);
  handler_trigger._pinch = TriggerPinch(FLAGS_frame_width, FLAGS_frame_height);
  handler_trigger._wait = TriggerWait(FLAGS_frame_width, FLAGS_frame_height, -1);
  int choice_trigger;

  InitiatorHandler handler_initiator;
  handler_initiator._default = InitiatorDefault();
  handler_initiator._twohand = InitiatorTwoHand();
  int choice_initiator;

  while(true) {

    std::cout << "Pick initiator (1 : Default, 2: Two-Hand): ";
    std::cin >> choice_initiator;
    handler_initiator._choice = choice_initiator;

    std::cout << "Pick anchor (1 : Dynamic, 2: static): ";
    std::cin >> choice_anchor;
    handler_anchor._choice = choice_anchor;

    std::cout << "Pick trigger (1: Thumb of base palm, 2: Thumb of free palm, 3: Pinch with free palm, 4: Wait to select): ";
    std::cin >> choice_trigger;
    handler_trigger._choice = choice_trigger;
    handler_trigger._wait.choice = choice_anchor;

    if (choice_anchor == 1 && choice_initiator == 2) {
      std::cerr << "two-hand initiator cannot be used with dynamic anchor\n";
      continue;
    }

    ::mediapipe::Status run_status = RunMPPGraph(
        handler_initiator,
        handler_anchor,
        handler_trigger,
        FLAGS_calculator_graph_config_file, 
        FLAGS_input_video_path, 
        FLAGS_output_video_path, 
        FLAGS_frame_width, 
        FLAGS_frame_height, 
        FLAGS_fps,
        FLAGS_debug);
    
    if (!run_status.ok()) {
      LOG(ERROR) << "Failed to run the graph: " << run_status.message();
      // return EXIT_FAILURE;
    } else {
      LOG(INFO) << "Success!";
    }
  }

  return EXIT_SUCCESS;
}
