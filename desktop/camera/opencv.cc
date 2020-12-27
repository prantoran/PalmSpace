
#include "camera.h"

#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h" // GUI #include "opencv2/highgui/highgui.hpp"


CameraOpenCV::CameraOpenCV() {
    m_valid = true;
    m_width = 640;
    m_height = 480;
    m_fps = 15;

    // capture = cv::VideoCapture(0);
    m_capture.open(0);

    #if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
    m_capture.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
    m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
    m_capture.set(cv::CAP_PROP_FPS, m_fps);
    #endif
}


CameraOpenCV::~CameraOpenCV() {

}


void CameraOpenCV::rgb(cv::Mat & dst) {
    m_capture >> dst;
}


void CameraOpenCV::depth(cv::Mat & dst) {
    return;
}


bool CameraOpenCV::isOpened() {
    return m_capture.isOpened();
}

void CameraOpenCV::get_frames() {
    return;
}

float CameraOpenCV::get_depth(int x_col, int y_row) {
    return -1;
}