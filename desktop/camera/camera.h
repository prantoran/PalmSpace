

#ifndef CAMERA_H
#define CAMERA_H

#include "mediapipe/framework/port/opencv_highgui_inc.h" // GUI #include "opencv2/highgui/highgui.hpp"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <iostream>


class Camera {
    public:
    int m_width, m_height;
    bool m_valid;
    int m_fps; // frame_rate
    virtual void get_frames() = 0;
    virtual void rgb(cv::Mat & dst) = 0;
    virtual void depth(cv::Mat & dst) = 0;
    virtual bool isOpened() = 0;
    virtual float get_depth(int x_col, int y_row) = 0;
    int get_fps();
    bool is_valid();
};


// use OpenCV capture
class CameraOpenCV: public Camera {
    cv::VideoCapture m_capture;
    public:
    CameraOpenCV(int _width, int _height, int _fps);
    ~CameraOpenCV();
    void get_frames();
    void rgb(cv::Mat & dst);
    void depth(cv::Mat & dst);
    bool isOpened();
    float get_depth(int x_col, int y_row);
};


// use RealSense SDK
class CameraRealSense: public Camera {
    rs2::pipeline m_pipe;
    rs2::frameset m_frameset;

    // https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1frame.html
    rs2::frame m_rsColor, m_rsDepth_color;
    rs2::frame m_rsDepth;
    rs2::colorizer m_color_map;
    rs2::pipeline_profile m_profile;
    rs2_stream m_align_to;
    // rs2::align m_align;
    
    float m_depth_scale;
    // rs2::depth_frame m_rsDepth_2;
    // Declare depth colorizer for pretty visualization of depth data
    cv::Mat m_depth_mat;

    std::vector<cv::Mat> m_frames_color;
    int m_frames_size;

    public:
    CameraRealSense(int _width, int _height, int _fps);
    ~CameraRealSense();
    void get_frames();
    void rgb(cv::Mat & dst);
    void depth(cv::Mat & dst);
    bool isOpened();
    void try_execute();
    float get_depth(int x_col, int y_row);
};



#endif