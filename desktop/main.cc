
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.

#include "desktop/anchors/anchors.h"
#include "desktop/triggers/triggers.h"
#include "desktop/initiators/initiators.h"
#include "desktop/handlers/handlers.h" 
#include "desktop/ui/menu.h"
#include "desktop/config/config.h"

#include "desktop/config/choices.h"
#include "desktop/camera/camera.h"

#include "desktop/userstudies/trial.h"

// #include <tensorflow/cc/saved_model/loader.h>
// #include <tensorflow/cc/saved_model/tag_constants.h>

// #include "mediapipe/framework/port/rs.hpp" // Include RealSense Cross Platform API


#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif



DEFINE_string(
    calculator_graph_config_file, 
    "mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt",
    "Name of file containing text format CalculatorGraphConfig proto.");
DEFINE_string(input_video_path, "",
              "Full path of video to load. "
              "If not provided, attempt to use a webcam.");
DEFINE_string(output_video_path, 
              "demo/",
              "Full path of where to save result (.mp4 only). "
              "If not provided, show result in a window.");

DEFINE_int32(frame_width, 640, "frame/screen width in pixels."); // 640 // 1280 // 848
DEFINE_int32(frame_height, 480, "frame/screen height in pixels."); // 480 // 720 // 480
DEFINE_int32(fps, 30, "frames per second.");
DEFINE_int32(debug, 0, "debug mode");


std::shared_ptr<MediaPipeMultiHandGPU> mp_graph = NULL;


std::string current_time() {
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,sizeof(buffer),"%d_%m_%Y_%H_%M_%S",timeinfo);
  return std::string(buffer);
}


std::string get_current_dir() {
   char buff[FILENAME_MAX]; //create string buffer to hold path
   GetCurrentDir( buff, FILENAME_MAX );
   std::string current_working_dir(buff);
   return current_working_dir;
}


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
  // AVX2 is faster than SSE3 cpu instruction set

  for (const auto& f: features) {
    std::cout << f << ". " << names[f] << " " << (cv::checkHardwareSupport(f)? "enabled": "disabled") << "\n";
  }

  std::cout << "Number of logical CPUs available to OpenCV: " << cv::getNumberOfCPUs() << "\n";
  std::cout << "Number of threads available to OpenCV: " << cv::getNumThreads() << "\n";
  std::cout << "OpenCV is optimized code enabled: " << (cv::useOptimized()? "yes": "no") << "\n";
}

int main(int argc, char** argv) {
  /* initialize random seed: */
  srand (time(NULL));

  checkOpenCVHardwareSupport();

  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  const bool load_video = !FLAGS_input_video_path.empty();
  const bool save_video = !FLAGS_output_video_path.empty();

  // analogous to structural pattern
   
  if (mp_graph == NULL) {
    mp_graph = std::make_shared<MediaPipeMultiHandGPU>(
      APP_NAME, 
      FLAGS_output_video_path + "/video" + current_time() + ".mp4");    
    try {
      auto tst = cv::Scalar(25,25,25);
    } catch (const std::exception& e) {
      std::cout << "exception tst:" << e.what() << "\n";
    } 
  }

  
  int choice_anchor = 5;
  int choice_trigger = 5; // 5 tap z 6 tap depth cam 8 dwell
  int choice_initiator = 1;
  int choice_divisions = 5;
  int choice_screensize = 2;
  int choice_debug = 1;
  int choice_visibility = 2;
  int choice_depth = 1;
  int choice_trial_start_btn_location = 4;
  bool choice_trial_pause_before_each_target = true;
  bool choice_trial_show_button_during_trial = false;

  #ifndef REALSENSE_CAM
    if (choice_depth == 8)
      choice_depth = 0;
  #endif

  PalmSpaceUI::Menu menu = PalmSpaceUI::Menu(
    FLAGS_frame_width,
    FLAGS_frame_height,
    choice_anchor,
    choice_trigger,
    choice_divisions,
    choice_divisions,
    choice_screensize,
    choice_visibility,
    choice_debug,
    choice_depth,
    choice_trial_start_btn_location,
    choice_trial_pause_before_each_target,
    choice_trial_show_button_during_trial,
    APP_NAME);

  menu.run();

  menu.get_choices(
    choice_initiator,
    choice_anchor,
    choice_trigger,
    choice_divisions,
    choice_screensize,
    choice_visibility,
    choice_debug,
    choice_depth,
    choice_trial_start_btn_location,
    choice_trial_pause_before_each_target,
    choice_trial_show_button_during_trial);
  

  mp_graph->trial = new userstudies::Trial(
    choice_divisions, 
    FLAGS_frame_width,
    FLAGS_frame_height);

  switch (choice_trial_start_btn_location) {
    case 1:
      mp_graph->trial->m_start_btn_loc = userstudies::Location::LEFT;
      break;
    case 2:
      mp_graph->trial->m_start_btn_loc = userstudies::Location::CENTER;
      break;
    case 3:
      mp_graph->trial->m_start_btn_loc = userstudies::Location::LEFTCENTER;
    case 4:
      mp_graph->trial->m_start_btn_loc = userstudies::Location::RIGHTCENTER;
  } 

  mp_graph->trial->m_trial_pause_before_each_target = choice_trial_pause_before_each_target;
  mp_graph->trial->m_trial_show_button_during_trial = choice_trial_show_button_during_trial;


  mp_graph->trial->generate_sample_space();
  mp_graph->trial->generate_random_target_sequence(7);

  std::cout << "frame_width:" << FLAGS_frame_width << " frame_height:" << FLAGS_frame_height << "\n";

  if (choice_depth) {
    #ifdef REALSENSE_CAM
      mp_graph->camera = new CameraRealSense(
        FLAGS_frame_width,
        FLAGS_frame_height,
        FLAGS_fps
      );
    #else 
      mp_graph->camera = new CameraOpenCV(
        FLAGS_frame_width,
        FLAGS_frame_height,
        FLAGS_fps
      );  
    #endif  
  } else {
    mp_graph->camera = new CameraOpenCV(
      FLAGS_frame_width,
      FLAGS_frame_height,
      FLAGS_fps
    );
  }

  switch (choice_initiator) {
    case 1:
      mp_graph->initiator = new InitiatorDefault();
      break;
    case 2:
      mp_graph->initiator = new InitiatorTwoHand();
      break;
    default:
      std::cout << "ERROR main.cc invalid initiator choice\n";
      return EXIT_FAILURE;
  }

  switch (choice_anchor) {
    case 1:
      mp_graph->anchor = new AnchorDynamic(cv::Scalar(25, 25, 255), cv::Scalar(255, 25, 25));
      break;
    case 2:
      mp_graph->anchor = new AnchorStatic(
        cv::Scalar(25, 25, 255), 
        cv::Scalar(255, 25, 25), 
        get_current_dir() + "/desktop/anchors/Hand.png");
      break;
    case 3:
      mp_graph->anchor = new AnchoHandToScreen(
        cv::Scalar(25, 25, 255), 
        cv::Scalar(255, 25, 25), 
        get_current_dir() + "/desktop/anchors/Hand.png");
      break;
    case 4:
      mp_graph->anchor = new AnchorPad(
        cv::Scalar(25, 25, 255), 
        cv::Scalar(255, 25, 25), 
        get_current_dir() + "/desktop/anchors/Hand.png");
      break;
    case 5:
      mp_graph->anchor = new AnchorPadLarge(
        FLAGS_frame_width,
        FLAGS_frame_height,
        cv::Scalar(25, 25, 255), 
        cv::Scalar(255, 25, 25), 
        get_current_dir() + "/desktop/anchors/Hand.png",
        get_current_dir() + "/desktop/anchors/bg.jpg");
      break;
    default:
      std::cout << "ERROR main.cc invalid anchor choice\n";
      return EXIT_FAILURE;
  }

  mp_graph->anchor->setDivisions(choice_divisions);
    
  std::cout << "choice_trigger:" << choice_trigger << "\n";

  switch (choice_trigger) {
    case 1:
      mp_graph->trigger = new TriggerThumb(FLAGS_frame_width, FLAGS_frame_height);
      break;
    case 2:
      mp_graph->trigger = new TriggerThumbOther(FLAGS_frame_width, FLAGS_frame_height);
      break;
    case 3:
      mp_graph->trigger = new TriggerPinch(FLAGS_frame_width, FLAGS_frame_height);
      break;
    case 4:
      mp_graph->trigger =  new TriggerWait(FLAGS_frame_width, FLAGS_frame_height, -1);
      // mp_graph->trigger->set_anchor_choice(choice_anchor); // TODO inspect, needed by wait trigger
      break;
    case 5:
      mp_graph->trigger = new TriggerTap();
      break;
    case 6:
      mp_graph->trigger = new TriggerDwell();
      break;
    case 7:
      mp_graph->trigger = new TriggerTapDepthArea(
                                load_video, 
                                save_video, 
                                FLAGS_fps,
                                FLAGS_frame_width, 
                                FLAGS_frame_height);
      break;
    case 8:
      // mp_graph->trigger = new TriggerTapDepth();
      mp_graph->trigger = new TriggerTapDepthGradient();
      break;
    case 9:
      mp_graph->trigger = new TriggerTapDepthSingle();
      break;
    case 10:
      mp_graph->trigger = new TriggerTapDepthDistance();
      break;
    default:
      std::cout << "invalid trigger choice\n";
      return EXIT_FAILURE;
  }
  
  choices::eScreenSize ssize = choices::getScreenSize(choice_screensize);
  
  choices::eVisibility _visibility = choices::getVisibility(choice_visibility);

  mp_graph->anchor->setScreenSize(ssize);
  mp_graph->anchor->setVisibility(_visibility);

  if (choice_anchor == 1 && choice_initiator == 2) {
    // throw std::invalid_argument("")
    exit(-1);
  }

  std::cout << "setting up strict: whether or not to look for index pointer from 2nd hand only\n";
  if (choice_trigger == 1 || choice_trigger == 5) {
    mp_graph->initiator->strict = true; // only look for pointer in second hand
  } else if (choice_trigger == 6) {
    std::cout << "given dwell trigger, checking choice_anchor\n";
    if (choice_anchor != 3) {      
      mp_graph->initiator->strict = true; // only look for pointer in second hand
    }
  }

  ::mediapipe::Status run_status = mp_graph->run(
      FLAGS_calculator_graph_config_file, 
      FLAGS_input_video_path, 
      FLAGS_frame_width, 
      FLAGS_frame_height, 
      FLAGS_fps,
      choice_debug,
      load_video,
      save_video); 
  
  if (!run_status.ok()) {
    LOG(ERROR) << "Failed to run the graph: " << run_status.message();
    return EXIT_FAILURE; // probably defined in MediaPipe:ports
  } else {
    LOG(INFO) << "Success!";
  }

  return EXIT_SUCCESS; // probably defined in MediaPipe:ports
}

