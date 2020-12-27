
// #include "camera.h"
// // #include <opencv2/photo.hpp>

// CameraRealSense::CameraRealSense() {
//   m_width = 640;
//   m_height = 480;


//   rs2::config cfg;
//   cfg.enable_stream(RS2_STREAM_DEPTH, m_width, m_height, RS2_FORMAT_Z16);
//   cfg.enable_stream(RS2_STREAM_COLOR, m_width, m_height, RS2_FORMAT_BGR8);
//   // cfg.enable_stream(RS2_STREAM_INFRARED, 1, m_width, m_height, RS2_FORMAT_Y8);
//   // cfg.enable_stream(RS2_STREAM_INFRARED, 2, m_width, m_height, RS2_FORMAT_Y8);

//   // rs2::colorizer color_map;

//   // cfg.enable_stream(RS2_STREAM_DEPTH);
//   // cfg.enable_stream(RS2_STREAM_COLOR);

 


//   // Declare RealSense pipeline, encapsulating the actual device and sensors
//   // Start streaming with default recommended configuration
//   // pipe.start();
//   m_pipe.start(cfg);
// }


// CameraRealSense::~CameraRealSense() {

// }

// void CameraRealSense::get_frame() {
//   m_frames = m_pipe.wait_for_frames(); // Wait for next set of frames from the camera    
  
//   // for realigning depth image to rgb image
//   rs2::align align_to_color(RS2_STREAM_COLOR);
//   // Align to depth 
//   m_frames = align_to_color.process(m_frames);
  

// }


// void CameraRealSense::rgb(cv::Mat & dst) {

//   m_rsColor = m_frames.get_color_frame();
//   dst = cv::Mat(cv::Size(m_width, m_height),CV_8UC3, (void*) m_rsColor.get_data(), cv::Mat::AUTO_STEP);
//   // cv::fastNlMeansDenoisingColored(dst, dst, 10, 10, 7, 21);
// }


// void CameraRealSense::depth(cv::Mat & dst) {
//   m_rsDepth = m_frames.get_depth_frame();
//   m_rsDepth_color = m_rsDepth.apply_filter(m_color_map);

//   // dst = cv::Mat(cv::Size(m_width, m_height),CV_16U, (void*) m_rsDepth.get_data(), cv::Mat::AUTO_STEP);
//   // dst.convertTo(dst, CV_8UC1, 255.0/1000);

//   dst = cv::Mat(cv::Size(m_width, m_height), CV_8UC3, (void*) m_rsDepth_color.get_data(), cv::Mat::AUTO_STEP);

// }


// bool CameraRealSense::isOpened() {
//     return true;
// }


// void CameraRealSense::try_execute() {
// // TODO: 2020-11-29 wrap execution within try/catch using anonymous function pointers as arguments

// // try {}

// // // error
// // catch (const rs2::error & e)
// // {
// //     std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
// //     return EXIT_FAILURE;
// // }
// // catch (const std::exception& e)
// // {
// //     std::cerr << e.what() << std::endl;
// //     return EXIT_FAILURE;
// // }
// }


//   // const auto window_name_depth = "Depth Image";
//   // const auto window_name_rgb = "RGB Image";
//   // const auto window_name_infra_lft = "Infrared Left Image";
//   // const auto window_name_infra_rgt = "Infrared Right Image";


//   // cv::namedWindow(window_name_depth, cv::WINDOW_AUTOSIZE);
//   // cv::namedWindow(window_name_rgb, cv::WINDOW_AUTOSIZE);
//   // cv::namedWindow(window_name_infra_lft, cv::WINDOW_AUTOSIZE);
//   // cv::namedWindow(window_name_infra_rgt, cv::WINDOW_AUTOSIZE);



//   // cv::Mat depth, color;

// //   while (cv::waitKey(1) < 0 
// //     && cv::getWindowProperty(window_name_depth, cv::WND_PROP_AUTOSIZE) >= 0 
// //     && cv::getWindowProperty(window_name_rgb, cv::WND_PROP_AUTOSIZE) >= 0
// //     // && cv::getWindowProperty(window_name_infra_lft, cv::WND_PROP_AUTOSIZE) >= 0 
// //     // && cv::getWindowProperty(window_name_infra_rgt, cv::WND_PROP_AUTOSIZE) >= 0 
// // )
//   // {
//     // rs2::frameset frames = pipe.wait_for_frames(); // Wait for next set of frames from the camera
    
//     // // Align to depth 
//     // rs2::align align_to_color(RS2_STREAM_COLOR);
//     // frames = align_to_color.process(frames);

//     // rs2::frame rsDepth = frames.get_depth_frame().apply_filter(color_map);
//     // rs2::frame rsDepth = frames.get_depth_frame();
    
//     // rs2::frame rsColor = frames.get_color_frame();

//     // rs2::video_frame ir_frame_left = frames.get_infrared_frame(1);
//     // rs2::video_frame ir_frame_right = frames.get_infrared_frame(2);
        

//     // Query frame size (width and height)
//     // const int w_depth = rsDepth.as<rs2::video_frame>().get_width();
//     // const int h_depth = rsDepth.as<rs2::video_frame>().get_height();

//     // const int w_color = rsColor.as<rs2::video_frame>().get_width();
//     // const int h_color = rsColor.as<rs2::video_frame>().get_height();


//     // std::cerr << "rgb width:" << w_color << " height:" << h_color << "\n";
//     // std::cerr << "depth width:" << w_depth << " height:" << h_depth << "\n";

//     // Create OpenCV matrix of size (w,h) from the colorized depth data
//     // cv::Mat image(cv::Size(w, h), CV_8UC3, (void*)depth.get_data(), cv::Mat::AUTO_STEP);
//     // depth = cv::Mat(cv::Size(w_depth, h_depth),CV_16U, (void*) rsDepth.get_data(), cv::Mat::AUTO_STEP);
//     // depth.convertTo(depth, CV_8UC1, 255.0/1000);
//     // color = cv::Mat(cv::Size(w_color, h_color),CV_8UC3, (void*) rsColor.get_data(), cv::Mat::AUTO_STEP);
//     // cv::Mat color = cv::Mat(cv::Size(w_color, h_color), CV_8UC3);


//     // const int w_lft = ir_frame_left.as<rs2::video_frame>().get_width();
//     // const int h_lft = ir_frame_left.as<rs2::video_frame>().get_height();
//     // cv::Mat pic_left(cv::Size(w_lft,h_lft), CV_8UC1, (void*)ir_frame_left.get_data());
    
//     // const int w_rgt = ir_frame_right.as<rs2::video_frame>().get_width();
//     // const int h_rgt = ir_frame_right.as<rs2::video_frame>().get_height();
//     // cv::Mat pic_right(cv::Size(w_rgt,h_rgt), CV_8UC1, (void*)ir_frame_right.get_data());

//     // Update the window with new data
//     // cv::imshow(window_name_depth, depth);
//     // cv::imshow(window_name_rgb, color);
//     // cv::imshow(window_name_infra_lft, pic_left);    
//     // cv::imshow(window_name_infra_rgt, pic_right);

//   // }


