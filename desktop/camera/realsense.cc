
#include "camera.h"
// #include <opencv2/photo.hpp>


CameraRealSense::CameraRealSense(int _width, int _height, int _fps) {
  m_width = _width;
  m_height = _height;
  m_fps = _fps; // frame_rate, 15 and 30 seems to work, setting 60 distorts color image
  // increasing frame_rate increases depth quality but decrease color image quality
  // decreasing frame_rate opposite effect

  try {
    rs2::config cfg;

    cfg.enable_stream(RS2_STREAM_DEPTH, m_width, m_height, RS2_FORMAT_Z16, m_fps); // setting frame_rate without datatype does not work

    // having RS2_FORMAT_BGR8 is important or else the color of OpenCV matrix is bluish
    cfg.enable_stream(RS2_STREAM_COLOR, m_width, m_height, RS2_FORMAT_BGR8, m_fps); 

    std::cerr << "config set\n";
    
    // Declare RealSense pipeline, encapsulating the actual device and sensors
    // Start streaming with default recommended configuration
    
    // m_pipe.start();
    m_profile = m_pipe.start(cfg); // this is important or else the camera inputs become combined
    std::cerr << "pipeline started\n";

    auto sensor = m_profile.get_device().first<rs2::depth_sensor>();
    m_depth_scale =  sensor.get_depth_scale();

    if (sensor.supports(RS2_OPTION_ENABLE_AUTO_EXPOSURE)) {

      std::cout << "INFO: realsense depth sensor supports auto_exposure\n";
      sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
      std::cout << "INFO: enabled auto_exposure\n";
      rs2::roi_sensor s(sensor);
      rs2::region_of_interest roi = s.get_region_of_interest();
      
    } else {
      std::cout << "WARNING: realsense depth sensor does not support auto_exposure\n";
    }


    if (sensor.supports(RS2_OPTION_EMITTER_ENABLED)) {
      std::cout << "INFO: realsense depth sensor supports emitter\n";
      
      sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 1.f); // Enable emitter, required for more accuracy
      // sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f); // Disable emitter
    }

    if (sensor.supports(RS2_OPTION_LASER_POWER)) {
        // Query min and max values:
      std::cout << "INFO: realsense depth sensor supports laser power\n";
      
      // auto range = sensor.get_option_range(RS2_OPTION_LASER_POWER);
      // sensor.set_option(RS2_OPTION_LASER_POWER, range.max); // Set max power, what are the risks?
      // sensor.set_option(RS2_OPTION_LASER_POWER, 0.f); // Disable laser, reduces accuracy
    }


    std::cerr << "realsense pipe started, depth scale:" << std::to_string(m_depth_scale) << "\n";
    
    // https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1stream__profile.html
    std::vector<rs2::stream_profile> streams = m_profile.get_streams();

    bool found_depth = false, found_color = false;
    for (int i = 0; i < streams.size(); i ++) {
      std::cerr << "streams[" << i << "]: " << streams[i].stream_name() << "\n"; 
      if (streams[i].stream_type() == rs2_stream::RS2_STREAM_DEPTH) {
        found_depth = true;
      }

      if (streams[i].stream_type() == rs2_stream::RS2_STREAM_COLOR) {
        found_color = true;
        m_align_to = streams[i].stream_type();
      }
    }

    if (!found_color) {
      std::cout << "realsensense: color stream not found\n";
      // TODOD throw stream not found error
    }

    if (!found_depth) {
      std::cout << "realsensense: depth stream not found\n";
      // TODOD throw stream not found error
    }


    // m_align = rs2::align(m_align_to); // does not work

    m_frames_size = 3;
    m_frames_color = std::vector<cv::Mat> ();
    
    for (int i = 0; i < 2*m_fps; i ++) {
      //Wait for all configured streams to produce a frame
      m_frameset = m_pipe.wait_for_frames();
    }


  } catch(const std::exception& e) {
    std::cout << "realsense camera constructor error:" << e.what() << "\n";
  }
}


CameraRealSense::~CameraRealSense() {
  m_pipe.stop();
}

void CameraRealSense::get_frames() {

  m_valid = true;
  m_frameset = m_pipe.wait_for_frames(); // Wait for next set of frames from the camera    
  
  // for realigning depth image to rgb image
  rs2::align align_to(m_align_to);
  m_frameset = align_to.process(m_frameset);
  
  m_rsColor = m_frameset.get_color_frame();

  

  
  m_rsDepth = m_frameset.get_depth_frame();  


  // std::cerr << "color frame no:" << m_rsColor.get_frame_number() << "\tcolor timestamp:" << m_rsColor.get_timestamp() << "\n";
  // std::cerr << "color frame no:" << m_rsColor.get_frame_number() << "\tdepth frame no:" << m_rsDepth.get_frame_number() << "\n";

}


void CameraRealSense::rgb(cv::Mat & dst) {

  rs2::hole_filling_filter hole_filter = rs2::hole_filling_filter(2);
  m_rsColor.apply_filter(hole_filter);

  dst = cv::Mat(
    cv::Size(m_width, m_height),CV_8UC3, 
    (void*) m_rsColor.get_data(), 
    cv::Mat::AUTO_STEP);
  
  cv::medianBlur(dst, dst, 5);

  // m_frames_color.emplace_back(dst);
  // if (m_frames_color.size() == m_frames_size) {
  //   // cv::fastNlMeansDenoisingColoredMulti(
  //   //   m_frames_color,
  //   //   dst,
  //   //   m_frames_color.size()/2,
  //   //   m_frames_color.size()
  //   // );
    
  //   m_frames_color.erase(m_frames_color.begin());

  //   // cv::fastNlMeansDenoisingColored(dst, dst, 10, 10, 7, 21);
  // }
  
}


void CameraRealSense::depth(cv::Mat & dst) {
  m_rsDepth_color = m_rsDepth.apply_filter(m_color_map);

  // dst = cv::Mat(cv::Size(m_width, m_height),CV_16U, (void*) m_rsDepth.get_data(), cv::Mat::AUTO_STEP);
  // dst.convertTo(dst, CV_8UC1, 255.0/1000);

  // m_depth_mat = cv::Mat(cv::Size(m_width, m_height), CV_16UC1, (void*) (*m_depth_frame).get_data(), cv::Mat::AUTO_STEP);
  m_depth_mat = cv::Mat(cv::Size(m_width, m_height), CV_16UC1, (void*) m_rsDepth.get_data(), cv::Mat::AUTO_STEP);
  // dst = cv::Mat(cv::Size(m_width, m_height), CV_8UC3, (void*) m_rsDepth.get_data(), cv::Mat::AUTO_STEP);
  dst = cv::Mat(cv::Size(m_width, m_height), CV_8UC3, (void*) m_rsDepth_color.get_data(), cv::Mat::AUTO_STEP);
  
  // Apply Histogram Equalization
  // https://stackoverflow.com/questions/15007304/histogram-equalization-not-working-on-color-image-opencv
  if (dst.channels() == 1) {
    cv::equalizeHist(dst, dst);
  } else {
    // could not discern effect
    cv::Mat ycrcb;
    cv::cvtColor(dst, ycrcb, CV_BGR2YCrCb);
    std::vector<cv::Mat> channels;
    cv::split(ycrcb, channels);
    cv::equalizeHist(channels[0], channels[0]);
    cv::merge(channels, ycrcb);
    cv::cvtColor(ycrcb, dst, CV_YCrCb2BGR);
  }  
}


bool CameraRealSense::isOpened() {
    return true;
}


float CameraRealSense::get_depth(int x_col, int y_row) {
  // TODO
  rs2::depth_frame dpt_frame = m_rsDepth.as<rs2::depth_frame>();
  float pixel_distance_in_meters = dpt_frame.get_distance(x_col, y_row);
  return pixel_distance_in_meters;
}


void CameraRealSense::try_execute() {
// TODO: 2020-11-29 wrap execution within try/catch using anonymous function pointers as arguments

// try {}

// // error
// catch (const rs2::error & e)
// {
//     std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
//     return EXIT_FAILURE;
// }
// catch (const std::exception& e)
// {
//     std::cerr << e.what() << std::endl;
//     return EXIT_FAILURE;
// }
}




