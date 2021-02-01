
#ifndef UI_H
#define UI_H

#include "mediapipe/framework/port/opencv_imgproc_inc.h" // most likely contains headers for cv::types


#define WINDOW_NAME "PalmSpace CVUI"

namespace ui {
    void rounded_rectangle( cv::Mat& src, cv::Point topLeft, cv::Point bottomRight, const cv::Scalar lineColor, const int thickness, const int lineType , const int cornerRadius);
}

#endif