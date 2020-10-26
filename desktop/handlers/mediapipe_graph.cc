
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.

#include "handlers.h"
#include "../config/colors.h"
#include <tuple>

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h" // GUI #include "opencv2/highgui/highgui.hpp"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"


// data structures for mediapipe graph
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/framework/formats/detection.pb.h"


// #include <opencv2/opencv.hpp>


// constexpr allows compiler to run statement/function at compile time.
constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";
constexpr char trackbarWindowName[] = "Index Marker Tuning";
// constexpr char kWindowName[] = "PalmSpace";
constexpr int BLOB_AREA_THRESH = 500;
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
  window_name = _window_name;
  // curImageID = 1;
}

::mediapipe::Status MediaPipeMultiHandGPU::run(
    const std::string& calculator_graph_config_file,
    const std::string& input_video_path,
    const std::string& output_video_path,
    const int frame_width,
    const int frame_height,
    const int fps, 
    const int debug_mode,
    const int dev_video) {

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
  cv::VideoCapture capture;
  const bool load_video = !input_video_path.empty();
  if (load_video) {
    // capture.open(input_video_path); // read a video file
  } else {
    capture = cv::VideoCapture(dev_video);
    capture.open(0);
  RET_CHECK(capture.isOpened());

  }

  cv::VideoWriter writer;
  const bool save_video = !output_video_path.empty();
  if (!save_video) {
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    if (!load_video) {
      #if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
          capture.set(cv::CAP_PROP_FRAME_WIDTH, frame_width);
          capture.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height);
          capture.set(cv::CAP_PROP_FPS, fps);
      #endif
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
  bool grab_frames = true;
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
  
  // for storing key values to be passed among handlers
  auto params = ExtraParameters();
  params.reset();

  cv::Mat camera_frame_raw, camera_frame, output_frame_mat, hsv, hsv2;

  std::vector<std::vector<std::tuple<double, double, double>>> points(3); 
  
  for (int i = 0; i < 2; i ++) {
    points[i] = std::vector<std::tuple<double, double, double>> (21);
  }
  
  points[2].push_back(std::make_tuple(-1, -1, 0));

  // int minH = 130, maxH = 160, minS = 10, maxS = 40, minV = 75, maxV = 130;

  cv::namedWindow(trackbarWindowName, cv::WINDOW_AUTOSIZE);
  // cv::createTrackbar("MinH", trackbarWindowName, &minH, 239);
  // cv::createTrackbar("MaxH", trackbarWindowName, &maxH, 239);
  // cv::createTrackbar("MinS", trackbarWindowName, &minS, 255);
  // cv::createTrackbar("MaxS", trackbarWindowName, &maxS, 255);
  // cv::createTrackbar("MinV", trackbarWindowName, &minV, 255);
  // cv::createTrackbar("MaxV", trackbarWindowName, &maxV, 255);

  int minR = 233, maxR = 255, minG = 233, maxG = 255, minB = 233, maxB = 255;
  int minR2 = 0, maxR2 = 67, minG2 = 152, maxG2 = 255, minB2 = 208, maxB2 = 255;

  cv::createTrackbar("MinR", trackbarWindowName, &minR, 255);
  cv::createTrackbar("MaxR", trackbarWindowName, &maxR, 255);
  cv::createTrackbar("MinG", trackbarWindowName, &minG, 255);
  cv::createTrackbar("MaxG", trackbarWindowName, &maxG, 255);
  cv::createTrackbar("MinB", trackbarWindowName, &minB, 255);
  cv::createTrackbar("MaxB", trackbarWindowName, &maxB, 255);


// ffmpeg -i ggg.mp4 -r 15 -vf scale=512:-1 -ss 00:00:03 -to 00:00:06 opengl-rotating-triangle.gif

  while (grab_frames) {
    capture >> camera_frame_raw;
    if (camera_frame_raw.empty()) break;  // End of video.
    cv::cvtColor(camera_frame_raw, hsv, cv::COLOR_BGR2RGB);
    if (!load_video) {
      cv::flip(hsv, hsv, 1);
    }

    cv::inRange(hsv, cv::Scalar(minR, minG, minB), cv::Scalar(maxR, maxG, maxB), hsv);

    // int blurSize = 3;
    // int elementSize = 3;
    // cv::medianBlur(hsv, hsv, blurSize);
    // cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));
    // cv::dilate(hsv, hsv, element);


    cv::imshow(trackbarWindowName, hsv);
    if (cv::waitKey(30) >= 0) break;
  }

  cv::destroyWindow(trackbarWindowName);

  while (grab_frames) {
    capture >> camera_frame_raw;
    
    // if (camera_frame_raw->empty()) break;  // End of video.
    if (camera_frame_raw.empty()) break;  // End of video.

    // Converts an image from one color space to another.
    // cv::cvtColor(camera_frame_raw, hsv, CV_BGR2HSV);

    cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
    cv::cvtColor(camera_frame_raw, hsv, cv::COLOR_BGR2RGB);
    cv::cvtColor(camera_frame_raw, hsv2, cv::COLOR_BGR2RGB);


    if (!load_video) {
      cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);
      cv::flip(hsv, hsv, 1);
      cv::flip(hsv2, hsv2, 1);
    }

    // cv::inRange(hsv, cv::Scalar(minH, minS, minV), cv::Scalar(maxH, maxS, maxV), hsv);
    cv::inRange(hsv, cv::Scalar(minR, minG, minB), cv::Scalar(maxR, maxG, maxB), hsv);
    cv::inRange(hsv2, cv::Scalar(minR2, minG2, minB2), cv::Scalar(maxR2, maxG2, maxB2), hsv2);

    cv::Moments m1 = cv::moments(hsv);
    cv::Moments m2 = cv::moments(hsv2);

    cv::Point pt1(m1.m10/m1.m00, m1.m01/m1.m00);
    cv::Point pt2(m2.m10/m2.m00, m2.m01/m2.m00);

    // std::cerr << "centroid 1: (" << pt1.x << "," << pt1.y << ") centroid 2: (" << pt2.x << ", " << pt2.y << ")\n";


    int area1 = cv::countNonZero(hsv);
    int area2 = cv::countNonZero(hsv2);

    std::cout << "area1:" << area1 << " area2:" << area2 << "\n";

    cv::addWeighted( hsv, 1, hsv2, 1, 0.0, hsv);
    cv::imshow(trackbarWindowName, hsv);

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


    /*
      points[2] used for sending auxiliary information
      points[2][0] = {indexfinger_x, indexfinger_y, cz};
    */

    // resetting points
    for (int i = 0; i < 2; i ++) {
      points[i][0] = std::make_tuple(0, 0, 0);
      for (int j = 1; j < 21; j ++) {
        points[i][j] = points[i][0];
      }
    }

    int hand_index = 0;

    for (const auto& hand_landmarks : multi_hand_landmarks) {
      if (hand_index == 2) break; // considering only two hands
      
      int j = 0;
      
      for (const auto& landmark : hand_landmarks.landmark()) {
        points[hand_index][j] = std::make_tuple(landmark.x(), landmark.y(), landmark.z());
      
        ++ j;
      }

      ++hand_index;
    }

    points[2][0] = std::make_tuple(indexfinger_x, indexfinger_y, 0);

    
    // params.reset();

    initiator.params(points, params);

    params.get_palmbase(palmbase);
    params.get_indexbase(indexbase);
    
    if (initiator.inspect(points)) {
      show_display = true;
      initiator.params(points, params);

      params.get_palmbase(palmbase);
      params.get_indexbase(indexbase);


    } else {
      params.reset();
      show_display = false;
    }
    
    // Convert back to opencv for display or saving.
    // cv::Mat output_frame_mat = mediapipe::formats::MatView(output_frame.get());
    output_frame_mat = camera_frame;
    cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);
    
    if (!width) {
        width = output_frame_mat.size().width;
        height = output_frame_mat.size().height;
    }

    {
      // indexfinger_x = -1;
      // indexfinger_y = -1;

      if (params.is_set_indexfinger()) {
        otherindex_x_prv = otherindex_x;
        otherindex_y_prv = otherindex_y;
        
        params.get_indexfinger(otherindex_x, otherindex_y);

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
      output_frame_mat, 
      palmbase,
      indexbase, 
      interface_scaling_factor,
      indexfinger_x, indexfinger_y, params.extra_params); // saves selected i,j in extra_params[7,8] 
    
    cv::Rect gg = anchor.getGrid();


    

    if (show_display || anchor.static_display()) {

          points[2][0] = std::make_tuple(indexfinger_x, indexfinger_y, 0); // putting (indexfinger_x, indexfinger_y) if in case trigger is wait
          trigger.update(points, params.extra_params);

          if (trigger.status() == TRIGGER::RELEASED) {
            anchor.highlightSelected();
          }
          
      anchor.draw(
          output_frame_mat, 
          palmbase,
          indexbase, 
          interface_scaling_factor,
          indexfinger_x, indexfinger_y, params.extra_params);

      if (params.is_set_indexfinger()) {
        cv::circle(
          output_frame_mat,
          cv::Point(indexfinger_x, indexfinger_y),
          10,
          COLORS_darkblue,
          -1,
          cv::LINE_8,
          0);
      }

      if (debug_mode == 1) {
        for (int i = 0; i < points[0].size(); i ++) {
          cv::putText(output_frame_mat, //target image
          std::to_string(i), //text
          cv::Point(std::get<0>(points[0][i])*width, std::get<1>(points[0][i])*height),
          cv::FONT_HERSHEY_DUPLEX,
          1.0,
          CV_RGB(0, 0, 0), //font color
          2);
        }

        for (int i = 0; i < points[1].size(); i ++) {
          cv::putText(output_frame_mat, //target image
          std::to_string(i), //text
          cv::Point(std::get<0>(points[1][i])*width, std::get<1>(points[1][i])*height),
          cv::FONT_HERSHEY_DUPLEX,
          1.0,
          CV_RGB(0, 0, 0), //font color
          2);
        }
      }
    }


    if (area1 > BLOB_AREA_THRESH && pt1.x >= 0 && pt1.x >= 0 && pt1.x < frame_width && pt1.y < frame_height) {
      cv::circle(
        output_frame_mat,
        pt1,
        10,
        cv::Scalar(100, 100, 128),
        -1);

      std::string areamsg1 = "Area1:" + std::to_string(area1);
      cv::putText(output_frame_mat, //target image
            areamsg1, //text
            cv::Point(pt1.x+12, pt1.y), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            0.5,
            COLORS_darkslategrey, //font color
            INFO_TXT_BOLDNESS
        );

    }    

    if (area2 > BLOB_AREA_THRESH && pt2.x >= 0 && pt2.x >= 0 && pt2.x < frame_width && pt2.y < frame_height) {
      cv::circle(
        output_frame_mat,
        pt2,
        10,
        cv::Scalar(100, 128, 100),
        -1);

      std::string areamsg2 = "Area2:" + std::to_string(area2);
      cv::putText(output_frame_mat, //target image
            areamsg2, //text
            cv::Point(pt2.x+12, pt2.y), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            0.5,
            COLORS_darkslategrey, //font color
            INFO_TXT_BOLDNESS
        );
    }

    // debugging
    // cv::circle(
    //     output_frame_mat,
    //     anchor.getGridTopLeft(),
    //     10,
    //     cv::Scalar(10, 10, 10),
    //     -1);

    if ((area1 > BLOB_AREA_THRESH && pt1.x >= 0 && pt1.x >= 0 && pt1.x < frame_width && pt1.y < frame_height)) {

      if ((area2 > BLOB_AREA_THRESH && pt2.x >= 0 && pt2.x >= 0 && pt2.x < frame_width && pt2.y < frame_height)) {
        

        cv::line( output_frame_mat, pt1, pt2, cv::Scalar( 128, 128, 128 ), 1 );

        int dx2 = (pt1.x-pt2.x)*(pt1.x-pt2.x);
        int dy2 = (pt1.y-pt2.y)*(pt1.y-pt2.y);

        int distxy = root(dx2 + dy2); 

        std::string distxy_msg2 = "Distance XY: ~" + std::to_string(distxy);
        cv::putText(output_frame_mat, //target image
                distxy_msg2, //text
                cv::Point((pt1.x+pt2.x)/2, (pt1.y+pt2.y)/2), //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                0.5,
                COLORS_darkslategrey, //font color
                INFO_TXT_BOLDNESS
            );
        
        if (points_ratio < 0) {
          points_ratio = (double)area1 / area2;
        }

        double cur_ratio = (double)area1 / area2;

        std::cerr << "points_ratio:" << points_ratio << " cur_ratio:" << cur_ratio << "\n"; 

        double depth_approx = (points_ratio-cur_ratio)*(points_ratio-cur_ratio)*100;

        std::string depth_msg2 = "Depth difference: ~" + std::to_string(depth_approx);
        cv::putText(output_frame_mat, //target image
                depth_msg2, //text
                cv::Point((pt1.x+pt2.x)/2, 20 + (pt1.y+pt2.y)/2), //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                0.5,
                COLORS_darkslategrey, //font color
                INFO_TXT_BOLDNESS
            );
      }
    } else {
      points_ratio = -1; // resetting relative ratio
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

    //   // if (!writer.isOpened()) {
    //   //   LOG(INFO) << "Prepare video writer.";
    //   //   writer.open(output_video_path,
    //   //               mediapipe::fourcc('a', 'v', 'c', '1'),  // .mp4
    //   //               capture.get(cv::CAP_PROP_FPS), output_frame_mat.size());
    //   //   RET_CHECK(writer.isOpened());
    //   // }
    //   // writer.write(output_frame_mat);
    //   cv::imwrite( output_video_path + "/" + imageName(curImageID, image_ext), output_frame_mat);
    //   curImageID ++;
    } else {

      cv::imshow(window_name, output_frame_mat);
      // Press any key to exit.
      const int pressed_key = cv::waitKey(5);
      if (pressed_key >= 0 && pressed_key != 255) grab_frames = false;
      
      
      // cv::imshow(kWindowName, output_frame_mat);
      // const int pressed_key = cv::waitKey(5);
      // if (pressed_key >= 0 && pressed_key != 255) grab_frames = false;
    }
  }

  LOG(INFO) << "Shutting down.";
  if (writer.isOpened()) writer.release();
  MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
  return graph.WaitUntilDone();
}


