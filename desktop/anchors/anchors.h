


#ifndef ANCHORS_H
#define ANCHORS_H
#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

constexpr double alpha = 0.4;
#define COLORS_floralwhite  cv::Scalar(240,250,255)

class Anchor { // interface via abstract class
    public:
    std::string name;
    int width = 0, height = 0, ws, hs, min_ws, min_hs;
    int gap, xs[11], ys[11], dx, dy;
    double palmbase_x, palmbase_y;
    double palmbase_momentum;
    cv::Scalar color_red, color_blue, color_green, color_cur;

    int selected_i, selected_j, selected_i_prv, selected_j_prv; 
    int green_i, green_j;
    std::string message, message_selected;

    bool static_display;

    int divisions;

    int progress_maxwidth, progress_maxheight; // progress bar
    int pwidth, npwidth;

    virtual ~Anchor();

    void setConfig(int _width, int _height);

    // pure virtual functions
    virtual std::string type() = 0;

    virtual void calculate(
        const cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params) = 0;
    
    virtual void draw(
        cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params) = 0;
    virtual void reset_palmbase() = 0;
    
    std::tuple<int, int> selectedIndexes();


    void highlightSelected();
    void setDivisions(int _divisions);
    int getDivisions();
    void drawProgressBar(cv::Mat & _image, double _progress);

    void setupGrid();
    void setupSelection(int index_pointer_x, int index_pointer_y);
    void drawTextHighlighted(cv::Mat & overlay);
    void drawTextSelected(cv::Mat & overlay);
};

class AnchorDynamic: public Anchor {
    double palmbase_x_prv, palmbase_y_prv;
    public:
    AnchorDynamic();
    AnchorDynamic(cv::Scalar red, cv::Scalar blue);
    std::string type();
    
    void calculate(
        const cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params);

    void draw(
        cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double area, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params);
    
    void reset_palmbase();
};

class AnchorStatic: public Anchor {
    public:
    AnchorStatic();
    AnchorStatic(cv::Scalar red, cv::Scalar blue);
    std::string type();

    cv::Mat image_palm, mask;

    int palm_ubx, palm_uby;
    int palmstart_x, palmstart_y;

    void calculate(
        const cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params);

    void draw(
        cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params);
    
    void reset_palmbase();
};

class AnchorMidAir: public Anchor {
    public:
    AnchorMidAir();
    AnchorMidAir(cv::Scalar red, cv::Scalar blue);
    std::string type();

    int palmstart_x, palmstart_y;

    void calculate(
        const cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params);

    void draw(
        cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params);
    
    void reset_palmbase();
};

#endif