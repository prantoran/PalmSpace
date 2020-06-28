
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.

#include "handlers.h"
#include <cstdlib>
#include <tuple>

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"

#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/framework/formats/detection.pb.h"


// constexpr allows compiler to run statement/function at compile time.
constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";
constexpr char kWindowName[] = "PalmSpace";

cv::Scalar color_red = cv::Scalar(25, 25, 255);
cv::Scalar color_green = cv::Scalar(25, 255, 25);
cv::Scalar color_blue = cv::Scalar(255, 25, 25);
cv::Scalar color_dark_blue = cv::Scalar(69, 1, 1);

::mediapipe::Status RunMPPGraph(
    InitiatorHandler& initiator,
    AnchorHandler& anchor, 
    TriggerHandler& trigger,
    const std::string& calculator_graph_config_file,
    const std::string& input_video_path,
    const std::string& output_video_path,
    const int frame_width,
    const int frame_height,
    const int fps, 
    const int debug_mode) {

  std::string calculator_graph_config_contents;
  MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
      calculator_graph_config_file, &calculator_graph_config_contents));
  LOG(INFO) << "Get calculator graph config contents: "
            << calculator_graph_config_contents;
  mediapipe::CalculatorGraphConfig config =
      mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
          calculator_graph_config_contents);

  LOG(INFO) << "Initialize the calculator graph.";
  mediapipe::CalculatorGraph graph;
  MP_RETURN_IF_ERROR(graph.Initialize(config));

  LOG(INFO) << "Initialize the GPU.";
  ASSIGN_OR_RETURN(auto gpu_resources, mediapipe::GpuResources::Create());
  MP_RETURN_IF_ERROR(graph.SetGpuResources(std::move(gpu_resources)));
  mediapipe::GlCalculatorHelper gpu_helper;
  gpu_helper.InitializeForTest(graph.GetGpuResources().get());

  LOG(INFO) << "Initialize the camera or load the video.";
  cv::VideoCapture capture;
  const bool load_video = !input_video_path.empty();
  if (load_video) {
    capture.open(input_video_path);
  } else {
    capture.open(0);
  }
  // std::cerr << "capture.isOpened():" << capture.isOpened() << "\n";
  RET_CHECK(capture.isOpened());

  cv::VideoWriter writer;
  const bool save_video = !output_video_path.empty();
  if (!save_video) {
    cv::namedWindow(kWindowName, /*flags=WINDOW_AUTOSIZE*/ 1);
  // std::cerr << "CV VERSION:" << CV_VERSION << " MAJOR_VERSION:" << CV_MAJOR_VERSION << " MINOR_VERSION:" << CV_MINOR_VERSION << " CV_SUBMINOR_VERSION:" << CV_SUBMINOR_VERSION << "\n";
  // std::cerr << "frame_width:" << frame_width << " frame_height:" << frame_height << " fps:" << fps << "\n";
#if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
    capture.set(cv::CAP_PROP_FRAME_WIDTH, frame_width);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height);
    capture.set(cv::CAP_PROP_FPS, fps);
#endif
  }

  LOG(INFO) << "Start running the calculator graph.";
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                   graph.AddOutputStreamPoller(kOutputStream));
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller multi_hand_landmarks_poller,
                  graph.AddOutputStreamPoller("multi_hand_landmarks"));
  
  
  MP_RETURN_IF_ERROR(graph.StartRun({}));

  LOG(INFO) << "Start grabbing and processing frames.";
  bool grab_frames = true;
  bool show_display = false;

  int cx = -1, cy = -1;
  double alpha = 0.7;
  cv::Mat overlay;

  double palmbase_x, palmbase_y;
  double interface_scaling_factor = 1;
  // used for smoothing
  double otherindex_x, otherindex_y, otherindex_x_prv, otherindex_y_prv;
  double otherindex_momentum = 0.2;

  int width = 0, height = 0;

  cv::Scalar color_cur;
  

  bool triggered = false;
  std::string trig_msg = "";

  std::tuple<int, int> selected;

  std::cerr << "finished initializing application variables.\n";

  while (grab_frames) {
    // Capture opencv camera or video frame.
    cv::Mat camera_frame_raw;
    capture >> camera_frame_raw;
    if (camera_frame_raw.empty()) break;  // End of video.
    cv::Mat camera_frame;
    // Converts an image from one color space to another.
    cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
    if (!load_video) {
      cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);
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

    // std::unique_ptr<mediapipe::ImageFrame> output_frame;
    // {
    //   // Get the graph result packet, or stop if that fails.
    //   mediapipe::Packet packet;
    //   // check if data exist from graph
    //   if (!poller.Next(&packet)) break;

    //   // Convert GpuBuffer to ImageFrame.
    //   MP_RETURN_IF_ERROR(gpu_helper.RunInGlContext(
    //       [&packet, &output_frame, &gpu_helper]() -> ::mediapipe::Status {
    //         auto& gpu_frame = packet.Get<mediapipe::GpuBuffer>();
    //         auto texture = gpu_helper.CreateSourceTexture(gpu_frame);
    //         output_frame = absl::make_unique<mediapipe::ImageFrame>(
    //             mediapipe::ImageFormatForGpuBufferFormat(gpu_frame.format()),
    //             gpu_frame.width(), gpu_frame.height(),
    //             mediapipe::ImageFrame::kGlDefaultAlignmentBoundary);
    //         gpu_helper.BindFramebuffer(texture);
    //         const auto info =
    //             mediapipe::GlTextureInfoForGpuBufferFormat(gpu_frame.format(), 0);
    //         glReadPixels(0, 0, texture.width(), texture.height(), info.gl_format,
    //                     info.gl_type, output_frame->MutablePixelData());
    //         glFlush();
    //         texture.Release();
    //         return ::mediapipe::OkStatus();
    //       }));
    // }
    
    mediapipe::Packet multi_hand_landmarks_packet;
    // check if landmarks exist from graph
    if (!multi_hand_landmarks_poller.Next(&multi_hand_landmarks_packet)) break;
    const auto& multi_hand_landmarks = multi_hand_landmarks_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();

    std::vector<std::vector<std::tuple<double, double, double>>> points(3); 
    /*
      points[2] used for sending auxiliary information
      points[2][0] = {cx, cy, cz};
    */
    points[2].push_back(std::make_tuple(cx, cy, 0));
    
    std::tuple<double, double, double> cur;
    
    int hand_index = 0;

    for (const auto& hand_landmarks : multi_hand_landmarks) {
      if (hand_index == 2) break; // considering only two hands
      for (const auto& landmark : hand_landmarks.landmark()) {
        cur = std::make_tuple(landmark.x(), landmark.y(), landmark.z());
        points[hand_index].push_back(cur);
      }
      ++hand_index;
    }

    // std::cerr << "done extractring landmarks\n";

    // std::cerr << "area0:" << areas[0] << " area1:" << areas[1] << "\n";
    
    std::vector<double> extra_params = {-1, -1, -1, -1, -1, -1};
    /*
      0: min_ws
      1: min_hs
      2: palmbase_x
      3: palmbase_y
      4: otherindex_x
      5: otherindex_y
    */

    if (initiator.inspect(points)) {
      show_display = true;
      extra_params = initiator.params(points);
      palmbase_x = extra_params[2];
      palmbase_y = extra_params[3];
    } else {
      show_display = false;
      anchor.reset_palmbase();
    }

    // std::cerr << "passed initiator\n";
    
    // std::cerr << "areaID:" << areaID << " otherPalmID:" << otherPalmID << " ptIDOtherIndex:" << ptIDOtherIndex<< "\n";

    // Convert back to opencv for display or saving.
    // cv::Mat output_frame_mat = mediapipe::formats::MatView(output_frame.get());
    cv::Mat output_frame_mat = camera_frame;
    cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);

    if (show_display || anchor.static_display()) {
    // if (show_display) {

        points[2][0] = std::make_tuple(cx, cy, 0); // putting (cx, cy) if in case trigger is wait
        trigger.update(points);

        if (trigger.status() == TRIGGER::RELEASED) {
          triggered = true;
          selected = anchor.selectedIndexes();
          // std::tuple<int, int> selected = anchor.selectedIndexes();

          trig_msg = "none";
          int cur = 3*(std::get<1>(selected) - 1) + std::get<0>(selected);
          if (cur > 0) {
            trig_msg = std::to_string(cur);
          }

          std::cerr << "trig_msg:" << trig_msg << "\n";
          anchor.setGreen(std::get<0>(selected), std::get<1>(selected));
        }
        
        if (!width) {
            width = output_frame_mat.size().width;
            height = output_frame_mat.size().height;
        }

        if (extra_params[4] != -1) {
            // std::cerr << "drawing finger\n";
            otherindex_x_prv = otherindex_x;
            otherindex_x = extra_params[4];
            if (otherindex_x_prv == -1) {
              otherindex_x_prv = otherindex_x;
            }
            otherindex_x = (1-otherindex_momentum)*otherindex_x + otherindex_momentum*otherindex_x_prv;
            

            otherindex_y_prv = otherindex_y;
            otherindex_y = extra_params[5];
            if (otherindex_y_prv == -1) {
              otherindex_y_prv = otherindex_y;
            }
            otherindex_y = (1-otherindex_momentum)*otherindex_y + otherindex_momentum*otherindex_y_prv;

            cx = otherindex_x*width;
            cy = otherindex_y*height;
            
        } else {
            cx = -1;
            cy = -1;
        }

        std::cerr << "multihand_gpu: cx:" << cx << " cy:" << cy  << "\n";
      // std::cerr << "about to call transform\n";
        overlay = anchor.transform(
            output_frame_mat, 
            palmbase_x, 
            palmbase_y, 
            interface_scaling_factor,
            cx, cy, extra_params);
      // std::cerr << "done callin transform\n";

      if (extra_params[4] != -1) {
        cv::circle(
          overlay,
          cv::Point(cx, cy),
          10,
          color_dark_blue,
          -1,
          cv::LINE_8,
          0);
      }

      if (debug_mode == 1) {
        for (int i = 0; i < points[0].size(); i ++) {
          cv::putText(overlay, //target image
          std::to_string(i), //text
          cv::Point(std::get<0>(points[0][i])*width, std::get<1>(points[0][i])*height),
          cv::FONT_HERSHEY_DUPLEX,
          1.0,
          CV_RGB(0, 0, 0), //font color
          2);
        }
      }

      // std::cerr << "ex:" << points[areaID][9].x*width << "," << points[areaID][9].y*height << "\n";

      cv::addWeighted(overlay, alpha, output_frame_mat, 1-alpha, 0, output_frame_mat);
    }

    if (triggered) {
      // std::cerr << "trigger message:" << trig_msg << "\n";
      cv::putText(output_frame_mat, //target image
        trig_msg, //text
        cv::Point(width/3, height - 20), //top-left position
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        color_green, //font color
        3);
    }
    
    if (save_video) {
      if (!writer.isOpened()) {
        LOG(INFO) << "Prepare video writer.";
        writer.open(output_video_path,
                    mediapipe::fourcc('a', 'v', 'c', '1'),  // .mp4
                    capture.get(cv::CAP_PROP_FPS), output_frame_mat.size());
        RET_CHECK(writer.isOpened());
      }
      writer.write(output_frame_mat);
    } else {
      cv::imshow(kWindowName, output_frame_mat);
      // Press any key to exit.
      const int pressed_key = cv::waitKey(5);
      if (pressed_key >= 0 && pressed_key != 255) grab_frames = false;
    }
  }

  LOG(INFO) << "Shutting down.";
  if (writer.isOpened()) writer.release();
  MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
  return graph.WaitUntilDone();
}

