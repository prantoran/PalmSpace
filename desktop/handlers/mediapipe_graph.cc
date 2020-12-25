
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.

#include "handlers.h"
#include "../config/colors.h"
#include <tuple>

// opencv
#include "mediapipe/framework/port/opencv_highgui_inc.h" // GUI #include "opencv2/highgui/highgui.hpp"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"


// data structures for mediapipe graph
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/framework/formats/detection.pb.h"

#include "desktop/ui/menu.h"

// #include <opencv2/opencv.hpp>


// constexpr allows compiler to run statement/function at compile time.
constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";

// constexpr char kWindowName[] = "PalmSpace";
constexpr int BLOB_AREA_THRESH = 100;
constexpr int INFO_TXT_BOLDNESS = 2;


unsigned int root(unsigned int x){
    unsigned int a,b;
    b     = x;
    a = x = 0x3f;
    x     = b/x;
    a = x = (x+a)>>1;
    x     = b/x;
    a = x = (x+a)>>1;
    x     = b/x;
    x     = (x+a)>>1;
    return(x);  
}


MediaPipeMultiHandGPU::MediaPipeMultiHandGPU(const std::string & _window_name) {
  m_window_name = _window_name;
  // curImageID = 1;
  camera = NULL;
}


MediaPipeMultiHandGPU::~MediaPipeMultiHandGPU() {
  if (camera != NULL) {
    delete camera;
  }

  if (trigger != NULL) {
    delete trigger;
  }

  if (cv::getWindowProperty(m_window_name, 0) >= 0) {
    cv::destroyWindow(m_window_name);
  }
}


void MediaPipeMultiHandGPU::debug(
  cv::Mat & m_primary_output, 
  std::vector<std::vector<std::tuple<double, double, double>>> & points,
  ExtraParameters & params) {
    
  // // drawing landmarks of hand 0
  // for (int i = 0; i < points[0].size(); i ++) {
  //   cv::putText(m_primary_output, //target image
  //     std::to_string(i), //text
  //     cv::Point(std::get<0>(
  //       points[0][i])*params.m_frame_width, 
  //       std::get<1>(points[0][i])*params.m_frame_height),
  //     cv::FONT_HERSHEY_DUPLEX,
  //     1.0,
  //     CV_RGB(0, 0, 0), //font color
  //     2);
  // }

  // // drawing landmarks of hand 1
  // for (int i = 0; i < points[1].size(); i ++) {
  //   cv::putText(m_primary_output, //target image
  //     std::to_string(i), //text
  //     cv::Point(
  //       std::get<0>(points[1][i])*params.m_frame_width, 
  //       std::get<1>(points[1][i])*params.m_frame_height
  //       ),
  //     cv::FONT_HERSHEY_DUPLEX,
  //     1.0,
  //     CV_RGB(0, 0, 0), //font color
  //     2);
  // }


  int row_base, col_base;
  params.get_indexbase_cv_indices(row_base, col_base);
  // std::cerr << "tap base row:" << row_base << " col:" << col_base << "\n";

  int row_cursor, col_cursor;
  params.get_primary_cursor_cv_indices(row_cursor, col_cursor);
  // std::cerr << "tap cursor row:" << row_cursor << " col:" << col_cursor << "\n";

  // cv::circle(
  //   m_primary_output,
  //   cv::Point(row_base, col_base),
  //   10,
  //   cv::Scalar(0, 0, 255),
  //   cv::FILLED,
  //   cv::LINE_8
  // );


  cv::circle(
    m_primary_output,
    cv::Point(row_cursor, col_cursor),
    10,
    cv::Scalar(0, 255, 0),
    cv::FILLED,
    cv::LINE_8
  );


  // for (int k = 0, i = 10; i < 15; i += 4, k ++) {
  //       cv::circle(
  //         m_primary_output,
  //         cv::Point(
  //           params.m_frame_width * std::get<0>(points[0][i]), 
  //           params.m_frame_height * std::get<1>(points[0][i])),
  //         10,
  //         cv::Scalar(240,255,240),
  //         cv::FILLED,
  //         cv::LINE_8
  //       );
  // }

  cv::rectangle(
    m_primary_output,
    cv::Point(params.m_flood_width.m_lower_bound, params.m_flood_height.m_lower_bound),
    cv::Point(params.m_flood_width.m_upper_bound, params.m_flood_height.m_upper_bound),
    cv::Scalar(255, 255, 0), 
    2, 
    8, 
    0
  );

  cv::circle(
          m_primary_output,
          cv::Point(params.m_row, params.m_col),
          10,
          cv::Scalar(0,215,255),
          cv::FILLED,
          cv::LINE_8
        );
  

  // int xcol_palmid, yrow_palmid;
  // params.get_primary_cursor_middlefinger_base_cv_indices(xcol_palmid, yrow_palmid); 

  // cv::circle(
  //         m_primary_output,
  //         cv::Point(xcol_palmid, yrow_palmid),
  //         30,
  //         cv::Scalar(0,215,255),
  //         cv::FILLED,
  //         cv::LINE_8
  //       );

  if (!m_depth_map.empty()) { 
    cv::rectangle(
      m_depth_map,
      cv::Point(params.m_flood_width.m_lower_bound, params.m_flood_height.m_lower_bound),
      cv::Point(params.m_flood_width.m_upper_bound, params.m_flood_height.m_upper_bound),
      cv::Scalar(255, 255, 0), 
      2, 
      8, 
      0
    );
  }
}


::mediapipe::Status MediaPipeMultiHandGPU::run(
    const std::string& calculator_graph_config_file,
    const std::string& input_video_path,
    const std::string& output_video_path,
    const int frame_width,
    const int frame_height,
    const int fps, 
    const int debug_mode,
    const bool load_video,
    const bool save_video) {

  std::string calculator_graph_config_contents;
  MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
      calculator_graph_config_file, &calculator_graph_config_contents));
  // LOG(INFO) << "Get calculator graph config contents: "
            // << calculator_graph_config_contents;
  mediapipe::CalculatorGraphConfig config =
      mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
          calculator_graph_config_contents);
  
  mediapipe::CalculatorGraph graph;

  LOG(INFO) << "Initialize the calculator graph.";
  MP_RETURN_IF_ERROR(graph.Initialize(config));

  LOG(INFO) << "Initialize the GPU.";
  ASSIGN_OR_RETURN(auto gpu_resources, mediapipe::GpuResources::Create());
  MP_RETURN_IF_ERROR(graph.SetGpuResources(std::move(gpu_resources)));
  mediapipe::GlCalculatorHelper gpu_helper;
  gpu_helper.InitializeForTest(graph.GetGpuResources().get());

  LOG(INFO) << "Initialize the camera or load the video.";
  // cv::VideoCapture capture;

  if (load_video) {
    // capture.open(input_video_path); // read a video file
  } else {
    RET_CHECK(camera->isOpened());
  }

  cv::VideoWriter writer;
  if (!save_video) {
    cv::namedWindow(m_window_name, cv::WINDOW_AUTOSIZE);
    if (!load_video) {
      // moved to camera/opencv
    } 
  }

  LOG(INFO) << "Start running the calculator graph.";
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                   graph.AddOutputStreamPoller(kOutputStream));
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller multi_hand_landmarks_poller,
                  graph.AddOutputStreamPoller("multi_hand_landmarks"));
  // ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller multi_hand_palm_detections_poller,
  //                 graph.AddOutputStreamPoller("palm_detections"));
  
  MP_RETURN_IF_ERROR(graph.StartRun({}));

  LOG(INFO) << "Start grabbing and processing frames.";
  m_grab_frames = true;
  bool show_display = false;

  int indexfinger_x = -1, indexfinger_y = -1;
  
  std::tuple<double, double, double> palmbase = std::make_tuple(-1, -1, -1);
  std::tuple<double, double, double> indexbase = std::make_tuple(-1, -1, -1);

  double interface_scaling_factor = 1;
  // used for smoothing
  double otherindex_x = -1, otherindex_y = -1, otherindex_x_prv, otherindex_y_prv;
  double otherindex_momentum = 0.8;
  double points_ratio = -1; // ratio of areas of 2 blobs represented by pixels of 2 different color ranges

  int width = 0, height = 0;

  cv::Scalar color_cur;
  
  std::string trig_msg = "";

  std::tuple<int, int> selected;

  const int divisions = anchor.getDivisions();
  

  cv::Mat camera_frame_raw, camera_frame;
  // for storing key values to be passed among handlers
  ExtraParameters params = ExtraParameters(frame_width, frame_height, load_video, camera);
  params.set_depth_map(&m_depth_map);
  
  std::vector<std::vector<std::tuple<double, double, double>>> points(3); 
  
  for (int i = 0; i < 2; i ++) {
    points[i] = std::vector<std::tuple<double, double, double>> (21);
  }
  
  points[2].push_back(std::make_tuple(-1, -1, 0));

  bool isDone = false;

  while (!isDone && m_grab_frames) {
    camera->get_frames();

    if (!camera->is_valid()) {
      std::cout << "camera input invalid\n";
      continue;
    }

    camera->depth(m_depth_map);
    // m_depth_map.convertTo(m_depth_map, CV_8UC1, 255.0/1000);
    camera->rgb(camera_frame_raw);

    // if (camera_frame_raw->empty()) break;  // End of video.
    if (camera_frame_raw.empty()) break;  // End of video.

    
    // Converts an image from one color space to another.
    // cv::cvtColor(camera_frame_raw, hsv, CV_BGR2HSV);

    cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);


    if (!params.load_video) {
      cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);
      cv::flip(m_depth_map, m_depth_map, 1);
    }

    // Wrap Mat into an ImageFrame.
    auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
        mediapipe::ImageFormat::SRGB, camera_frame.cols, camera_frame.rows,
        mediapipe::ImageFrame::kGlDefaultAlignmentBoundary);
    cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
    camera_frame.copyTo(input_frame_mat);

    // Prepare and add graph input packet.
    size_t frame_timestamp_us =
        (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
    MP_RETURN_IF_ERROR(
        gpu_helper.RunInGlContext([&input_frame, &frame_timestamp_us, &graph,
                                   &gpu_helper]() -> ::mediapipe::Status {
          // Convert ImageFrame to GpuBuffer.
          auto texture = gpu_helper.CreateSourceTexture(*input_frame.get());
          auto gpu_frame = texture.GetFrame<mediapipe::GpuBuffer>();
          glFlush();
          texture.Release();
          // Send GPU image packet into the graph.
          MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
              kInputStream, mediapipe::Adopt(gpu_frame.release())
                                .At(mediapipe::Timestamp(frame_timestamp_us))));
          return ::mediapipe::OkStatus();
        }));

    cv::cvtColor(camera_frame, camera_frame, cv::COLOR_RGB2BGR);

    {
      // needed block or else there is memory leak (~30MB)
      std::unique_ptr<mediapipe::ImageFrame> output_frame;
      // Get the graph result packet, or stop if that fails.
      mediapipe::Packet packet;
      // check if data exist from graph
      if (!poller.Next(&packet)) break;

      // Convert GpuBuffer to ImageFrame.
      MP_RETURN_IF_ERROR(gpu_helper.RunInGlContext(
          [&packet, &output_frame, &gpu_helper]() -> ::mediapipe::Status {
            auto& gpu_frame = packet.Get<mediapipe::GpuBuffer>();
            auto texture = gpu_helper.CreateSourceTexture(gpu_frame);
            output_frame = absl::make_unique<mediapipe::ImageFrame>(
                mediapipe::ImageFormatForGpuBufferFormat(gpu_frame.format()),
                gpu_frame.width(), gpu_frame.height(),
                mediapipe::ImageFrame::kGlDefaultAlignmentBoundary);
            gpu_helper.BindFramebuffer(texture);
            const auto info =
                mediapipe::GlTextureInfoForGpuBufferFormat(gpu_frame.format(), 0);
            glReadPixels(0, 0, texture.width(), texture.height(), info.gl_format,
                        info.gl_type, output_frame->MutablePixelData());
            glFlush();
            texture.Release();
            return ::mediapipe::OkStatus();
          }));
    }
    
    mediapipe::Packet multi_hand_landmarks_packet;
    // check if landmarks exist from graph
    if (!multi_hand_landmarks_poller.Next(&multi_hand_landmarks_packet)) break;
    const auto& multi_hand_landmarks = multi_hand_landmarks_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();

    // resetting points
    for (int i = 0; i < 2; i ++) {
      points[i][0] = std::make_tuple(0, 0, 0);
      for (int j = 1; j < 21; j ++) {
        points[i][j] = points[i][0];
      }
    }

    {
      int hand_id = 0;

      for (const auto& hand_landmarks : multi_hand_landmarks) {
        if (hand_id == 2) break; // considering only two hands

        int j = 0;        
        for (const auto& landmark : hand_landmarks.landmark()) {
          points[hand_id][j] = std::make_tuple(landmark.x(), landmark.y(), landmark.z());
          ++ j;
        }

        ++hand_id;
      }
    }

    points[2][0] = std::make_tuple(indexfinger_x, indexfinger_y, 0);

    initiator.params(points, params);

    params.get_palmbase(palmbase);
    params.get_indexbase(indexbase);
    
    params.set_is_static(anchor.static_display());

    show_display = false;

    if (initiator.inspect(points)) {
      show_display = true;
      initiator.params(points, params);

      params.get_palmbase(palmbase);
      params.get_indexbase(indexbase);


    } else if (!anchor.static_display()) {
      params.reset();
    }
    
    // Convert back to opencv for display or saving.
    // cv::Mat m_primary_output = mediapipe::formats::MatView(output_frame.get());
    if (anchor.type() == choices::anchor::MIDAIR) {
      try {
        m_primary_output = cv::Mat(
            camera_frame.rows, 
            camera_frame.cols, 
            CV_8UC3, 
            cv::Scalar(0, 0, 0));

      } catch(cv::Exception e) {
          std::cerr << "ERROR anchors/anchors_static.cpp draw() " << e.what() << "\n";
      }
    } else {
      m_primary_output = camera_frame;
    }
    
    
    if (!width) {
        width = camera_frame.size().width;
        height = camera_frame.size().height;
    }

    {
      if (params.is_set_primary_cursor()) {
        otherindex_x_prv = otherindex_x;
        otherindex_y_prv = otherindex_y;
        
        params.get_primary_cursor(otherindex_x, otherindex_y);

        if (otherindex_x_prv == -1) {
          otherindex_x_prv = otherindex_x;
        }

        if (otherindex_y_prv == -1) {
          otherindex_y_prv = otherindex_y;
        }

        if (otherindex_x != -1 && otherindex_y != -1) {
          otherindex_x = (1-otherindex_momentum)*otherindex_x + otherindex_momentum*otherindex_x_prv;
          otherindex_y = (1-otherindex_momentum)*otherindex_y + otherindex_momentum*otherindex_y_prv;

          indexfinger_x = otherindex_x*width;
          indexfinger_y = otherindex_y*height;
        }   
      }
    }
    
    anchor.calculate(
      camera_frame, 
      palmbase,
      indexbase, 
      interface_scaling_factor,
      indexfinger_x, indexfinger_y, params); // saves selected i,j in extra_params[7,8] 
    
    cv::Rect gg = anchor.getGrid();

    if (show_display || anchor.static_display()) {

      points[2][0] = std::make_tuple(indexfinger_x, indexfinger_y, 0); // putting (indexfinger_x, indexfinger_y) if in case trigger is wait
      trigger->update(camera_frame_raw, points, params);

      if (trigger->status() == TRIGGER::RELEASED) {
        anchor.highlightSelected();
      }
          
      anchor.draw(
          camera_frame, 
          m_primary_output,
          palmbase,
          indexbase, 
          interface_scaling_factor,
          indexfinger_x, indexfinger_y, params);

      if (params.is_set_primary_cursor()) {
        cv::circle(
          m_primary_output,
          cv::Point(indexfinger_x, indexfinger_y),
          10,
          COLORS_darkblue,
          -1,
          cv::LINE_8,
          0);
      }

      if (debug_mode == 1) {
        debug(m_primary_output, points, params);
      }
    }

    combine_output_frames();

    cv::imshow(m_window_name, m_combined_output);
    
    check_keypress();
    save_output();
  }

  LOG(INFO) << "Shutting down.";
  


  if (writer.isOpened()) writer.release();
  MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
  return graph.WaitUntilDone();
}


void MediaPipeMultiHandGPU::combine_output_frames() {
  if (m_depth_map.empty()) {
    m_combined_output = m_primary_output;
  } else {
    cv::Size sz1 = m_primary_output.size();
    cv::Size sz2 = m_depth_map.size();
    if (m_combined_output.empty() || 
      m_combined_output.size().width < (sz1.width + sz2.width)) {
      m_combined_output = cv::Mat(
        std::max(sz1.height, sz2.height),
        sz1.width + sz2.width,
        CV_8UC3);
      m_combined_output_left = cv::Mat(
        m_combined_output,
        cv::Rect(0, 0, sz1.width, sz1.height));
      m_combined_output_right = cv::Mat(
        m_combined_output,
        cv::Rect(sz1.width, 0, sz2.width, sz2.height));
    } 

    m_primary_output.copyTo(m_combined_output_left);
    m_depth_map.copyTo(m_combined_output_right);
  }
}


void MediaPipeMultiHandGPU::check_keypress() {
  // Press any key to exit.
  const int pressed_key = cv::waitKey(5);
  if (pressed_key >= 0 && pressed_key != 255) m_grab_frames = false;
}


void MediaPipeMultiHandGPU::save_output() {
  //   if (save_video) {

  // if (!writer.isOpened()) {
  //   LOG(INFO) << "Prepare video writer.";
  //   writer.open(output_video_path,
  //   mediapipe::fourcc('a', 'v', 'c', '1'),  // .mp4
  //   camera->get_fps(), m_primary_output.size());
  //   RET_CHECK(writer.isOpened());
  // }
  // writer.write(m_primary_output);

  // //   // if (!writer.isOpened()) {
  // //   //   LOG(INFO) << "Prepare video writer.";
  // //   //   writer.open(output_video_path,
  // //   //               mediapipe::fourcc('a', 'v', 'c', '1'),  // .mp4
  // //   //               capture.get(cv::CAP_PROP_FPS), m_primary_output.size());
  // //   //   RET_CHECK(writer.isOpened());
  // //   // }
  // //   // writer.write(m_primary_output);
  // //   cv::imwrite( output_video_path + "/" + imageName(curImageID, image_ext), m_primary_output);
  // //   curImageID ++;
  // } else {

}

