#include "camera.h"

int Camera::get_fps() {
    // return m_capture.get(cv::CAP_PROP_FPS);
    return m_fps;
}

bool Camera::is_valid() {
    return m_valid;
}