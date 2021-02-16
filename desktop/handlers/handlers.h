#ifndef HANDLERS_H
#define HANDLERS_H

#include "mediapipe/framework/port/status.h"
#include "desktop/anchors/anchors.h"
#include "desktop/triggers/triggers.h"
#include "desktop/initiators/initiators.h"
#include "desktop/config/config.h"
#include "desktop/config/choices.h"
#include "desktop/camera/camera.h"
#include "desktop/userstudies/trial.h"


#include <tuple>
#include <vector>
#include <queue>


class MediaPipeMultiHandGPU {
    public:
    Anchor * anchor;
    Initiator * initiator;
    Camera * camera; // TODO add m_
    Trigger * trigger;
    // int curImageID;
    std::string m_window_name;

    bool m_grab_frames;

    cv::Mat m_primary_output, m_combined_output;
    cv::Mat m_combined_output_left, m_combined_output_right;
    cv::Mat m_depth_map;
    
    cv::VideoWriter m_writer;
    std::string m_output_video_path;

    userstudies::Trial * trial;

    MediaPipeMultiHandGPU(const std::string & _window_name, const std::string & _output_video_path);
    ~MediaPipeMultiHandGPU();

    ::mediapipe::Status run(
        const std::string& calculator_graph_config_file,
        const std::string& input_video_path,
        const int frame_width,
        const int frame_height,
        const int fps,
        const int debug_mode,
        const bool load_video,
        const bool save_video); 
    
    void debug(
        cv::Mat & output_frame_mat, 
        std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);

    void combine_output_frames();
    void check_keypress();
};


namespace handlers {
namespace util {

// Remap x from range [lo hi] to range [0 1] then multiply by scale.
inline double Remap(double x, double lo, double hi, double scale);

void GetMinMaxZ(
    const std::vector<std::tuple<double, double, double>>& landmarks, 
    double* z_min, double* z_max);

void SetColorSizeValueFromZ(
  double z, double z_min, double z_max,
  int *color_scale,
  double min_depth_circle_thickness,
  double max_depth_circle_thickness,
  int *size_scale);


} // namespace util
} // namespace handlers


#endif