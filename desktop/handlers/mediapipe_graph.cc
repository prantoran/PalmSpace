
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.

#include "handlers.h"
#include "../config/colors.h"

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"

#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"

// data structures for mediapipe graph
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/classification.pb.h"

#include "desktop/ui/cvui.h"


// constexpr allows compiler to run statement/function at compile time.
constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";
constexpr char kLandmarkOutputStream[] = "hand_landmarks";
constexpr char kHandedness[] = "handedness";

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


MediaPipeMultiHandGPU::MediaPipeMultiHandGPU(
  const std::string & _window_name,
  const std::string & _output_video_path) {
  
  m_window_name = _window_name;
  m_output_video_path = _output_video_path;
  // curImageID = 1;
  camera = NULL;
  m_debug = false;

  std::cerr << "m_output_video_path:" << m_output_video_path << "\n";
}


MediaPipeMultiHandGPU::~MediaPipeMultiHandGPU() {
  std::cerr << "MediaPipeMultiHandGPU destructor called\n";
  if (camera != NULL) {
    delete camera;
  }

  if (trigger != NULL) {
    delete trigger;
  }

  if (anchor != NULL) {
    delete anchor;
  }

  if (initiator != NULL) {
    delete initiator;
  }

  if (cv::getWindowProperty(m_window_name, 0) >= 0) {
    cv::destroyWindow(m_window_name);
  }

  std::cerr << "MediaPipeMultiHandGPU destructor successful\n";
}


void MediaPipeMultiHandGPU::debug(
  cv::Mat & m_primary_output, 
  std::vector<std::vector<SmoothCoord>> & points,
  Parameters & params) {
    
  // drawing landmarks of hand 0
  for (int i = 0; i < points[0].size(); i ++) {
    cv::putText(m_primary_output, //target image
      std::to_string(i), //text
      cv::Point(
        points[0][i].x() * params.m_frame_width, 
        points[0][i].y() * params.m_frame_height),
      cv::FONT_HERSHEY_DUPLEX,
      1.0,
      CV_RGB(0, 0, 0), //font color
      2);
  }

  // drawing landmarks of hand 1
  for (int i = 0; i < points[1].size(); i ++) {
    cv::putText(m_primary_output, //target image
      std::to_string(i), //text
      cv::Point(
        points[1][i].x() * params.m_frame_width, 
        points[1][i].y() * params.m_frame_height
        ),
      cv::FONT_HERSHEY_DUPLEX,
      1.0,
      CV_RGB(255, 255, 255), //font color
      2);
  }

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

    if (anchor->m_type == choices::anchor::PADLARGE) {
      ui::clear_rectangle(
        m_depth_map,
        cv::Point(anchor->m_grid.m_x_cols[0],  anchor->m_grid.m_y_rows[0]),                            // start & end points 
        cv::Point(anchor->m_grid.m_x_cols[0],  anchor->m_grid.m_y_rows[0] + anchor->m_grid.m_height),
        cv::Point(anchor->m_grid.m_x_cols[0] + anchor->m_grid.m_width,      anchor->m_grid.m_y_rows[0] + anchor->m_grid.m_height),                            // start & end points 
        cv::Point(anchor->m_grid.m_x_cols[0] + anchor->m_grid.m_width,      anchor->m_grid.m_y_rows[0]),                            // start & end points 
        COLORS_floralwhite);
    }

    if (params.is_set_primary_cursor()) {
      cv::circle(
        m_depth_map,
        params.cursor_cvpoint(),
        params.primary_cursor_size(),
        // COLORS_darkblue,
        cv::Scalar(
          params.primary_cursor_color_size(), 
          params.primary_cursor_color_size(), 
          params.primary_cursor_color_size()            
        ),
        -1,
        cv::LINE_8,
        0);
    }
  }

  if (params.m_show_depth_txt) {
    int xcol, yrow;
    params.get_primary_cursor_cv_indices(xcol, yrow);
    cv::putText(
      m_primary_output,
      params.m_depth_txt,
      cv::Point(xcol, yrow),
      cv::FONT_HERSHEY_DUPLEX,
      1.0,
      cv::Scalar(139, 0, 139),
      2);
  }
}


::mediapipe::Status MediaPipeMultiHandGPU::run(
    const std::string& calculator_graph_config_file,
    const std::string& input_video_path,
    const int frame_width,
    const int frame_height,
    const int fps, 
    const int debug_mode,
    const bool load_video,
    const bool save_video) {

  m_debug = debug_mode;

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

  if (load_video) {
    // capture.open(input_video_path); // read a video file
  } else {
    RET_CHECK(camera->isOpened());
  }

  cv::VideoWriter writer;
  cv::namedWindow(m_window_name, cv::WINDOW_AUTOSIZE);
  if (!load_video) {
    // moved to camera/opencv
  } 

  LOG(INFO) << "Start running the calculator graph.";
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                   graph.AddOutputStreamPoller(kOutputStream));
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller multi_hand_landmarks_poller,
                  graph.AddOutputStreamPoller(kLandmarkOutputStream));
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller handedness_poller,
                  graph.AddOutputStreamPoller(kHandedness));
  // ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller multi_hand_palm_detections_poller,
  //                 graph.AddOutputStreamPoller("palm_detections"));
  

  

  MP_RETURN_IF_ERROR(graph.StartRun({}));

  m_grab_frames = true;
  bool show_display = false;
  
  // for storing key values to be passed among handlers
  Parameters params = Parameters(frame_width, frame_height, load_video, camera);
  params.set_depth_map(&m_depth_map);

  // used for smoothing
  double interface_scaling_factor = 1;

  cv::Scalar color_cur;
  
  std::string trig_msg = "";

  const int divisions = anchor->getDivisions();
  
  cv::Mat camera_frame;
  
  params.m_points = std::vector<std::vector<SmoothCoord>> (2);
  for (int i = 0; i < 2; i ++) {
    params.m_points[i] = std::vector<SmoothCoord> (21);
    for (int j = 0; j < 21; j ++) {
      params.m_points[i][j] = SmoothCoord(
        "H_" + std::to_string(i) + "L_" + std::to_string(j),
        0.999,
        0.2,
        5
      );
    }
  }

  std::vector<std::vector<std::tuple<double, double, double>>> points(2);
  for (int i = 0; i < 2; i ++) {
    points[i] = std::vector<std::tuple<double, double, double>> (21);
  }

  const int POINTS_RESET_THRESH = 10;
  int points_reset_counter[2] = {0, 0};

  
  bool isDone = false;

  LOG(INFO) << "Start grabbing and processing frames.";

  while (!isDone && m_grab_frames) {

    if (trial && trial->done()) {
      break;
    }

    camera->get_frames();

    if (!camera->is_valid()) {
      std::cout << "camera input invalid\n";
      continue;
    }

    camera->depth(m_depth_map);
    // m_depth_map.convertTo(m_depth_map, CV_8UC1, 255.0/1000);
    camera->rgb(camera_frame);


    if (anchor->m_type == choices::anchor::PADLARGE || 
        anchor->m_type == choices::anchor::HANDTOSCREEN) {
      camera_frame.copyTo(m_depth_map);
    }

    if (camera_frame.empty()) {
      std::cout << "camera frame empty\n"; 
      break;  // End of video.
    } 
    // Converts an image from one color space to another.
    // cv::cvtColor(camera_frame, hsv, CV_BGR2HSV);

    cv::cvtColor(camera_frame, camera_frame, cv::COLOR_BGR2RGBA);


    if (!params.load_video) {
      cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);
      cv::flip(m_depth_map, m_depth_map, 1);
    }



    // Wrap Mat into an ImageFrame.
    auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
        mediapipe::ImageFormat::SRGBA, camera_frame.cols, camera_frame.rows,
        mediapipe::ImageFrame::kGlDefaultAlignmentBoundary); 
    
    {
      // this code has effect, if commented out then landmarks are misaligned
      cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
      camera_frame.copyTo(input_frame_mat);
    }


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

    if (camera_frame.channels() == 4)
      cv::cvtColor(camera_frame, camera_frame, cv::COLOR_RGBA2BGR);
    else
      cv::cvtColor(camera_frame, camera_frame, cv::COLOR_RGB2BGR);

    {
      // needed block or else there is memory leak (~30MB)
      std::unique_ptr<mediapipe::ImageFrame> output_frame;
      // Get the graph result packet, or stop if that fails.
      mediapipe::Packet packet;
      // check if data exist from graph
      if (!poller.Next(&packet)) {
        continue;
      };

      // Convert GpuBuffer to ImageFrame.
      MP_RETURN_IF_ERROR(
        gpu_helper.RunInGlContext(
          [&packet, &output_frame, &gpu_helper]() -> ::mediapipe::Status {
            auto& gpu_frame = packet.Get<mediapipe::GpuBuffer>();
            auto texture = gpu_helper.CreateSourceTexture(gpu_frame);
            output_frame = absl::make_unique<mediapipe::ImageFrame>(
              mediapipe::ImageFormatForGpuBufferFormat(gpu_frame.format()),
              gpu_frame.width(), gpu_frame.height(),
              mediapipe::ImageFrame::kGlDefaultAlignmentBoundary
            );
            gpu_helper.BindFramebuffer(texture);
            const auto info =
                mediapipe::GlTextureInfoForGpuBufferFormat(gpu_frame.format(), 0);
            glReadPixels(
              0, 0, 
              texture.width(), texture.height(), 
              info.gl_format,
              info.gl_type, 
              output_frame->MutablePixelData()
            );
            glFlush();
            texture.Release();
            return ::mediapipe::OkStatus();
          }
        )
      );
    }


    mediapipe::Packet handedness_packet;
    bool _handedness_found = true;
    if (!handedness_poller.Next(&handedness_packet)) {
      // std::cout << "handler/MediaPipeMultiHandGPU::run() handedness cannot be polled\n";
      _handedness_found = false;
    }

    params.m_hand_mask = 0;

    if (_handedness_found) {

      const auto& handedness = handedness_packet.Get<std::vector<mediapipe::ClassificationList, std::allocator<mediapipe::ClassificationList> >>();
      
      for (int i = 0; i < 2; i ++) {
        params.hand[i] = handedness::UNDEFINED;
      }
      

      for (int i = 0; i < 2 && i < handedness.size(); i ++) {
        auto& h = handedness[i];
        // std::cerr << "h.classification_size:" << h.classification_size() << "\n";
        if (h.classification_size() > 0) {
          const mediapipe::Classification & c = h.classification(0);
          // std::cerr << "i:" << i << " label:" << c.label() << "\tindex:" << c.index() << "\n";
          
          switch (c.index()) {
            case 0:
              params.hand[i] = handedness::LEFT;
              break;
            case 1:
              params.hand[i] = handedness::RIGHT;
              break;
            default:
              std::cerr << "invalid handedness\n";
          }

          params.m_hand_mask |= (1 << (int)c.index());
        }
      }
    }
    
    mediapipe::Packet multi_hand_landmarks_packet;
    bool _landmarks_found = true;
    // check if landmarks exist from graph
    if (!multi_hand_landmarks_poller.Next(&multi_hand_landmarks_packet)) {
      // std::cout << "handler/MediaPipeMultiHandGPU::run() landmarks cannot be polled\n";
      _landmarks_found = false;
    }


    int hand_id = 0;

    // resetting points
    for (int i = 0; i < 2; i ++) {
      points[i][0] = std::make_tuple(0, 0, 0);
      for (int j = 1; j < 21; j ++) {
        points[i][j] = points[i][0];
      }

    } 

    for (int i = 0; i < 2; i ++) {
      if (!points_reset_counter[i]) {
        for (int j = 0; j < 21; j ++) {
          params.m_points[i][j].reset();
        }
      }
    }

    if (_landmarks_found) {
      
      const auto& multi_hand_landmarks = multi_hand_landmarks_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();

      for (const auto& hand_landmarks : multi_hand_landmarks) {
        if (hand_id == 2) break; // considering only two hands
        int phand_id = params.hand[hand_id];
        // std::cout << "hand_id:" << hand_id << "\tphand_id:" << phand_id << "\n";
        int j = 0;        
        for (const auto& landmark : hand_landmarks.landmark()) {
          
          points[phand_id][j] = 
            std::make_tuple(
              landmark.x(), 
              landmark.y(), 
              landmark.z()
            );

          ++ j;
        }

        // std::cout << "setting params.m_points\n";

        for (int i = 0; i < 21; i ++) {
          params.m_points[phand_id][i].set(
            points[phand_id][i]
          );
        }

        // std::cout << "setting params.m_points dpne\n";

        handlers::util::GetMinMaxZ(
          points[phand_id], 
          &params.m_hand_landmarks_relative_depth_minmax[phand_id].first,
          &params.m_hand_landmarks_relative_depth_minmax[phand_id].second);
        
        handlers::util::SetColorSizeValueFromZ(
          std::get<2>(points[phand_id][8]),
          params.m_hand_landmarks_relative_depth_minmax[phand_id].first,
          params.m_hand_landmarks_relative_depth_minmax[phand_id].second,
          &params.m_hand_color_scale[phand_id],
          1, 20,
          &params.m_hand_size_scale[phand_id]
        );

        ++hand_id;
      }


      // for (int i = 0; i < 2; i ++) {
      //   std::cout << "hand " << i << "\n";
      //   for (int j = 0 ; j < 21; j ++) {
      //     std::cout << "\t(" << params.m_points[i][j].x() << ", " << params.m_points[i][j].y() << ")\n";
      //   }
      // }

    }

    for (int i = 0; i < 2; i ++) {
      if (params.m_hand_mask & (1 << i)) {
        points_reset_counter[i] = POINTS_RESET_THRESH;
      } else {
        points_reset_counter[i] --;
        if (points_reset_counter[i] < 0) {
          points_reset_counter[i] = 0;
        }
      }
    }

    // std::cout << "hand_id:" << hand_id << "\n";
    // for (int i =  0; i < 2; i ++ ) {
    //   std::cout << "threshold :" << i << ": " << points_reset_counter[i] << "\n";
    // }

    auto sm_points = params.get_points();

    // std::cerr << "calling initiator->params(points, params)\n";
    // std::cerr << "initiator null? " << (initiator == NULL) << "\n";
    initiator->params(sm_points, params);

    params.set_is_static(anchor->m_static_display);

    show_display = false;

    if (initiator->inspect(sm_points, params)) {
      show_display = true;
      initiator->params(sm_points, params);

    } else if (!anchor->m_static_display) {
      params.reset();
    }


    
    
    if (anchor->type() == choices::anchor::HANDTOSCREEN || 
      anchor->type() == choices::anchor::PADLARGE) {
      m_primary_output = cv::Mat(
          frame_height, 
          frame_width, 
          CV_8UC3, 
          cv::Scalar(0, 0, 0));
    } else {
      m_primary_output = camera_frame;
    }
    // Convert back to opencv for display or saving.
    // cv::Mat m_primary_output = mediapipe::formats::MatView(output_frame.get());


    int cursor_x = -1, cursor_y = -1;

    if (params.is_set_primary_cursor()) {
      params.get_primary_cursor_cv_indices(cursor_x, cursor_y);
    } 
    
    anchor->calculate(
      camera_frame, 
      interface_scaling_factor,
      cursor_x, cursor_y, params); 

    
    if (show_display || anchor->m_static_display) {

      trigger->update(camera_frame, sm_points, params);

      switch (trigger->status()) {
        
        case TRIGGER::PRESSED:

          anchor->adjust_selection_prior_trigger();

          anchor->lock_selection();
        
        case TRIGGER::RELEASED:

          if (!trial || trial->started()) {
            
            anchor->markSelected();
            anchor->unlock_selection();
          }

          if (trial) {
            if (trial->process_is_button_clicked(cursor_x, cursor_y)) {
              anchor->reset_selection();
            }
            
            if (trial->matched(anchor->m_selected_i-1, anchor->m_selected_j-1)) {
              trial->process_correct_selection();
              anchor->unlock_selection();
            }
          }

          trigger->reset_status();
        default:
          anchor->unlock_selection();
          break;
      }
          
      anchor->draw(
          camera_frame, 
          m_primary_output,
          interface_scaling_factor,
          cursor_x, cursor_y, params);
      



      if (trial) {
        if (trial->started()) {
            trial->draw_target(m_primary_output, anchor->m_grid_out);
        }
      }
      if (anchor->m_type == choices::anchor::PADLARGE) {
        // trial->update_start_button_input_loc(anchor->m_grid);

        const cv::Point & index_tip = params.index_tip(); 
        const cv::Point & thumb_tip = params.thumb_tip();
        const cv::Point & thumb_base = params.thumb_base();

        trial->update_start_button_input_loc(
          index_tip, 
          cv::Point(
            std::max(index_tip.x + 100, thumb_tip.x + 10),
            std::max(index_tip.y + 100, std::max(thumb_base.y + 10, thumb_tip.y + 10)))
          );
        
        trial->draw_start_button(
          m_primary_output,
            cv::Point(
              anchor->m_grid_out.m_x_cols[0] + anchor->m_grid_out.m_width + 10,
              anchor->m_grid_out.m_y_rows[0] - anchor->m_grid_out.m_dy_row - 5
            ),
            cv::Point(
              anchor->m_grid_out.m_x_cols[0] + anchor->m_grid_out.m_width + 2*anchor->m_grid_out.m_dx_col,
              anchor->m_grid_out.m_y_rows[0]
            )
          );

        if (!m_depth_map.empty()) {
          trial->draw_start_button(
            m_depth_map,
              params.index_tip(), 
              cv::Point(
              std::max(index_tip.x + 100, thumb_tip.x + 10),
              std::max(index_tip.y + 100, std::max(thumb_base.y + 10, thumb_tip.y + 10))));
        }
      } else if (anchor->m_type == choices::anchor::HANDTOSCREEN) {
        
        trial->update_start_button_input_loc(
          cv::Point(
            anchor->m_grid_out.m_x_cols[0] + anchor->m_grid_out.m_width + 10,
            anchor->m_grid_out.m_y_rows[0] + anchor->m_grid_out.m_height/2 - anchor->m_grid_out.m_dy_row
          ),
          cv::Point(
            anchor->m_grid_out.m_x_cols[0] + anchor->m_grid_out.m_width + 2*anchor->m_grid_out.m_dx_col,
            anchor->m_grid_out.m_y_rows[0] + + anchor->m_grid_out.m_height/2 + anchor->m_grid_out.m_dy_row
          )
        );

        trial->draw_start_button(
          m_primary_output,
          cv::Point(
            anchor->m_grid_out.m_x_cols[0] + anchor->m_grid_out.m_width + 10,
            anchor->m_grid_out.m_y_rows[0] + anchor->m_grid_out.m_height/2 - anchor->m_grid_out.m_dy_row
          ),
          cv::Point(
            anchor->m_grid_out.m_x_cols[0] + anchor->m_grid_out.m_width + anchor->m_grid_out.m_dx_col,
            anchor->m_grid_out.m_y_rows[0] + + anchor->m_grid_out.m_height/2 + anchor->m_grid_out.m_dy_row
          )
        );
      } else {
        trial->update_start_button_input_loc(anchor->m_grid);
        trial->draw_start_button(m_primary_output);
      }


      if (anchor->m_type == choices::anchor::PADLARGE) {

        int g_c_x, g_c_y, go_c_x, go_c_y;

        anchor->m_grid.get_center_cv(g_c_x, g_c_y);
        anchor->m_grid_out.get_center_cv(go_c_x, go_c_y);

        int g_w   = anchor->m_grid.m_width;
        int g_h   = anchor->m_grid.m_height;
        int go_w  = anchor->m_grid_out.m_width;
        int go_h  = anchor->m_grid_out.m_height;

        int c_dx = cursor_x - g_c_x;
        int c_dy = cursor_y - g_c_y;

        c_dx = c_dx*((double)go_w/g_w);
        c_dy = c_dy*((double)go_h/g_h);
        
        int c_go_x = go_w + c_dx;
        int c_go_y = go_h + c_dy;

        if ((c_go_x >= 0 && c_go_x <= frame_width) &&
          (c_go_y >= 0 && c_go_y <= frame_height)) {
          
          cv::circle(
            m_primary_output,
            cv::Point(c_go_x, c_go_y),
            params.primary_cursor_size(),
            cv::Scalar(
              params.primary_cursor_color_size(), 
              params.primary_cursor_color_size(), 
              params.primary_cursor_color_size()            
            ),
            -1,
            cv::LINE_8,
            0  
          );
        }
      }

      
      if (params.is_set_primary_cursor() && anchor->m_type != choices::anchor::PADLARGE) {
        cv::circle(
          m_primary_output,
          cv::Point(cursor_x, cursor_y),
          params.primary_cursor_size(),
          // COLORS_darkblue,
          cv::Scalar(
            params.primary_cursor_color_size(), 
            params.primary_cursor_color_size(), 
            params.primary_cursor_color_size()            
          ),
          -1,
          cv::LINE_8,
          0);
      }

      if (debug_mode == 1) {
        if (!m_depth_map.empty()) {
          debug(m_depth_map, params.m_points, params);
        } else {
          debug(m_primary_output, params.m_points, params);
        }
      }
    }

    combine_output_frames();

    cv::imshow(m_window_name, m_combined_output);
    
    check_keypress();

    if (save_video) {
      if (!m_writer.isOpened()) {
        LOG(INFO) << "Prepare video writer.";
        m_writer.open(m_output_video_path,
          mediapipe::fourcc('a', 'v', 'c', '1'),  // .mp4
          std::min(15, camera->get_fps()), m_combined_output.size());
        RET_CHECK(m_writer.isOpened());
      }

      m_writer.write(m_combined_output);
    }
  }


  if (trial) {
    std::cerr << "time taken:\n";
    for (int i =  0 ; i < trial->m_target_sequence_size; i ++) {
      std::cerr << "target " << i << ": " << trial->m_time_taken[i].count() << "\n";
    }

    m_primary_output = cv::Scalar(79,79,79);
    for (int i = 0; i < trial->m_target_sequence_size; i ++) {
      std::string str = "target " + std::to_string(i) + ": " + std::to_string(trial->m_time_taken[i].count());
      std::cerr << "str:" << str << "\n";
      cvui::text(
        m_primary_output,
        20,
        i*20 + 20,
        str,
        0.5
      );
    }

    combine_output_frames();

    cv::imshow(m_window_name, m_combined_output);
    const int pressed_key = cv::waitKey(2000);
    if (pressed_key >= 0 && pressed_key != 255) m_grab_frames = false;

    if (save_video) {
      if (!m_writer.isOpened()) {
        LOG(INFO) << "Prepare video writer.";
        m_writer.open(m_output_video_path,
          mediapipe::fourcc('a', 'v', 'c', '1'),  // .mp4
          std::min(15, camera->get_fps()), m_combined_output.size());
        RET_CHECK(m_writer.isOpened());
      }
      for (int i = 0; i < 100; i ++) {
        m_writer.write(m_combined_output);

      }
    }
  }

  LOG(INFO) << "Shutting down.";

  if (m_writer.isOpened()) {
    m_writer.release();
  }

  MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
  return graph.WaitUntilDone();
}


void MediaPipeMultiHandGPU::combine_output_frames() {
  if (m_depth_map.empty() || !m_debug) {
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
