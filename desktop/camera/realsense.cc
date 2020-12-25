
#include "camera.h"
// #include <opencv2/photo.hpp>


CameraRealSense::CameraRealSense() {
  m_width = 640;
  m_height = 480;
  m_fps = 30; // frame_rate, 15 and 30 seems to work, setting 60 distorts color image
  // increasing frame_rate increases depth quality but decrease color image quality
  // decreasing frame_rate opposite effect

  try {
    rs2::config cfg;

    // cfg.enable_stream(RS2_STREAM_DEPTH);
    // cfg.enable_stream(RS2_STREAM_COLOR);
    cfg.enable_stream(RS2_STREAM_DEPTH, m_width, m_height, RS2_FORMAT_Z16, m_fps); // setting frame_rate without datatype does not work
    // cfg.enable_stream(RS2_STREAM_DEPTH, m_width, m_height, m_fps); 

    // having RS2_FORMAT_BGR8 is important or else the color of OpenCV matrix is bluish

    // cfg.enable_stream(RS2_STREAM_COLOR, m_width, m_height, RS2_FORMAT_BGR8); 
    cfg.enable_stream(RS2_STREAM_COLOR, m_width, m_height, RS2_FORMAT_BGR8, m_fps); 

    std::cerr << "enable stream configured\n";


    // Declare RealSense pipeline, encapsulating the actual device and sensors
    // Start streaming with default recommended configuration
    
    // m_pipe.start();
    m_profile = m_pipe.start(cfg); // this is important or else the camera inputs become combined

    auto sensor = m_profile.get_device().first<rs2::depth_sensor>();
    m_depth_scale =  sensor.get_depth_scale();


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
  
  // cv::fastNlMeansDenoisingColored(dst, dst, 10, 10, 7, 21);
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




