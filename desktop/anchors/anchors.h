


#ifndef ANCHORS_H
#define ANCHORS_H
#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"



class Anchor { // interface via abstract class
    public:
    std::string name;
    int width = 0, height = 0, ws, hs, min_ws, min_hs;
    int gap, xs[4], ys[4], dx, dy;
    double palmbase_x, palmbase_y;
    double palmbase_momentum;
    cv::Scalar color_red, color_blue, color_green, color_cur;

    int selected_i, selected_j, selected_i_prv, selected_j_prv; 
    int green_i, green_j;
    std::string message;

    bool static_display;

    virtual ~Anchor();

    // pure virtual functions
    virtual std::string type() = 0;
    virtual cv::Mat transform(
        const cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        const std::vector<double> & extra_params) = 0;
    virtual void reset_palmbase() = 0;
    
    std::tuple<int, int> selectedIndexes();

    void setGreen(int i, int j);
};

class AnchorDynamic: public Anchor {
    double palmbase_x_prv, palmbase_y_prv;
    public:
    AnchorDynamic();
    AnchorDynamic(cv::Scalar red, cv::Scalar blue);
    std::string type();
    cv::Mat transform(
        const cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double area, 
        int pointer_x, int pointer_y,
        const std::vector<double> & extra_params);
    void reset_palmbase();

};

class AnchorStatic: public Anchor {
    public:
    AnchorStatic();
    AnchorStatic(cv::Scalar red, cv::Scalar blue);
    std::string type();
    cv::Mat transform(
        const cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        const std::vector<double> & extra_params);
    void reset_palmbase();
};

#endif