
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.

#include "desktop/anchors/anchors.h"
#include "desktop/triggers/triggers.h"
#include "desktop/initiators/initiators.h"
#include "desktop/handlers/handlers.h" 
#include "desktop/ui/menu.h"
#include "desktop/camera/camera.h"

#include "desktop/config/config.h"
#include "desktop/config/colors.h"
#include "desktop/config/choices.h"

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

DEFINE_int32(frame_width, 640, "frame/screen width in pixels."); // 640 // 1280 // 848 // 1920
DEFINE_int32(frame_height, 480, "frame/screen height in pixels."); // 480 // 720 // 480 // 1080
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

bool is_number(const std::string& s) {
    
    std::cerr << "checking s: " << s << "\tlength:" << s.length()<<"\n"; 
    std::string::const_iterator it = s.begin();

    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
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

  int userID = -1;
  bool userid_found = false;
  while (!userid_found) {
    FILE * pipe = popen("zenity  --title  \"Insert User ID\" --entry --text \"Enter User ID here (natural numbers, no negatives, fractions, etc)\"", "r");
    if (!pipe)
    {
        std::cerr << "Couldn't start command." << std::endl;
        return 0;
    }

    std::array<char, 128> buffer;
    while (fgets(buffer.data(), 128, pipe) != NULL) {
        std::cout << "Reading userID..." << std::endl;
        std::string result = buffer.data();

        result.erase(
          std::remove(
            result.begin(), result.end(), '\n'
          ),
          result.end()
        );

        std::cerr << "userID:" << result << "\n";
        if (is_number(result)) {
          userID = std::atoi(result.c_str());
          userid_found = true;
        }

        break;
    }

    auto returnCode = pclose(pipe);
  } 
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

  
  int choice_anchor = 6;
  int choice_trigger = 5; // 5 tap z 6 tap depth cam 8 dwell
  int choice_initiator = 1;
  int choice_divisions = 4;
  int choice_screensize = 4;
  int choice_debug = 1;
  int choice_visibility = 1;
  int choice_depth = 1;
  int choice_trial_start_btn_location = 4;
  bool choice_trial_pause_before_each_target = true;
  bool choice_trial_show_button_during_trial = false;
  int choice_targets_cnt = 3;
  int choice_inputspace = 2;
  bool choice_practice = false;

  #ifndef REALSENSE_CAM
    if (choice_depth == 8)
      choice_depth = 0;
  #endif

  // bool valid = true;
  // while (valid) {
    PalmSpaceUI::Menu menu = PalmSpaceUI::Menu(
      FLAGS_frame_width,
      FLAGS_frame_height,
      choice_anchor,
      choice_trigger,
      choice_initiator,
      choice_divisions,
      choice_screensize,
      choice_visibility,
      choice_debug,
      choice_depth,
      choice_trial_start_btn_location,
      choice_trial_pause_before_each_target,
      choice_trial_show_button_during_trial,
      choice_targets_cnt,
      choice_inputspace,
      choice_practice,
      userID,
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
      choice_trial_show_button_during_trial,
      choice_targets_cnt,
      choice_inputspace,
      choice_practice);
    

    std::cerr << "choice_divisions:" << choice_divisions << "\n";

    // if (menu.m_exit) {
    //   // exit program
    //   valid = false;
    //   break;
    // }


    std::string technique_str, selection_str;

    switch (choice_anchor) {
      case 1:
        technique_str = "S2H_REL";
        break;
      case 3:
        technique_str = "S2H_ABS";
        break;
      case 5:
        technique_str = "H2S_ABS";
        break;
      case 6:
        technique_str = "H2S_REL";
        break;
      default:
        technique_str = "undefined";
        break;
    }

    switch (choice_trigger) {
      case 5:
        selection_str = "tap";
        break;
      case 6:
        selection_str = "dwell";
        break;
      default:
        selection_str = "undefined";
        break;
    }
    
    
    // s.init_file_with_headers();

    mp_graph->m_practice_mode = choice_practice;
    
    std::cerr << "choice_divisions:" << choice_divisions << "\n";

    mp_graph->study1 = new userstudies::Study1(
        userID,
        "desktop/userstudies/log/study1.csv",
        "desktop/userstudies/study1_trial_counter.txt",
        "desktop/userstudies/events/study1",
        technique_str,
        selection_str,
        choice_divisions
      );

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
    mp_graph->trial->init_datastores(choice_targets_cnt);

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
        // mp_graph->initiator = new InitiatorTwoHand();
        break;
      default:
        std::cout << "ERROR main.cc invalid initiator choice\n";
        return EXIT_FAILURE;
    }

    switch (choice_anchor) {
      case 1:
        mp_graph->anchor = new AnchorDynamic(COLORS_red, COLORS_royalblue);
        break;
      case 2:
        mp_graph->anchor = new AnchorStatic(
          COLORS_red, 
          COLORS_royalblue, 
          get_current_dir() + "/desktop/anchors/Hand.png");
        break;
      case 3:
        mp_graph->anchor = new AnchoHandToScreen(
          FLAGS_frame_width,
          FLAGS_frame_height,
          COLORS_red, 
          COLORS_royalblue, 
          get_current_dir() + "/desktop/anchors/Hand_full.png",
          get_current_dir() + "/desktop/anchors/bg.jpg");
        break;
      case 4:
        mp_graph->anchor = new AnchorPad(
          COLORS_red, 
          COLORS_royalblue, 
          get_current_dir() + "/desktop/anchors/Hand.png");
        break;
      case 5:
        // mp_graph->anchor = new AnchorPadLarge(
        //   FLAGS_frame_width,
        //   FLAGS_frame_height,
        //   COLORS_red, 
        //   COLORS_royalblue, 
        //   get_current_dir() + "/desktop/anchors/Hand.png",
        //   get_current_dir() + "/desktop/anchors/bg.jpg");
        mp_graph->anchor = new AnchorS2HAbsolute(COLORS_red, COLORS_royalblue);
        break;
      case 6:
        mp_graph->anchor = new AnchorH2SRelative(
          FLAGS_frame_width,
          FLAGS_frame_height, 
          COLORS_red, 
          COLORS_royalblue,
          get_current_dir() + "/desktop/anchors/Hand_full.png",
          get_current_dir() + "/desktop/anchors/bg.jpg");
          
        break;
      default:
        std::cout << "ERROR main.cc invalid anchor choice\n";
        return EXIT_FAILURE;
    }

    switch (choice_inputspace) {
      case 1:
        mp_graph->anchor->m_inputspace_type = choices::inputspace::SAMEASSCREENSIZE;
        break;
      case 2:
        mp_graph->anchor->m_inputspace_type = choices::inputspace::PALMSIZED;
        break;
      default:
        std::cerr << "ERROR main.cc invalid inputchoice\n";
        break;
    }

    mp_graph->anchor->setDivisions(choice_divisions);
      
    std::cout << "choice_trigger:" << choice_trigger << "\n";

    switch (choice_trigger) {
      case 5:
        mp_graph->trigger = new TriggerTap();
        break;
      case 6:
        mp_graph->trigger = new TriggerDwell();
        break;
      default:
        std::cout << "invalid trigger choice\n";
        return EXIT_FAILURE;
    }
    
    choices::screensize::types ssize = choices::screensize::from_int(choice_screensize);
    
    choices::visibility::types _visibility = choices::visibility::from_int(choice_visibility);

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

  // }

  return EXIT_SUCCESS; // probably defined in MediaPipe:ports
}

