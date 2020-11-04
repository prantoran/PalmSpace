


#ifndef ANCHORS_H
#define ANCHORS_H
#include <string>
#include <tuple>
#include <vector>
#include <iostream>

#include "mediapipe/framework/port/opencv_imgproc_inc.h" // most likely contains headers for cv::types

#include "desktop/config/colors.h"
#include "desktop/config/config.h"

constexpr double alpha = 0.4;
#define COLORS_floralwhite  cv::Scalar(240,250,255)


class ScreenSize {
    public:
    eScreenSize size;
    
    void setMinWidthHeight(
        int &min_width, 
        int &min_height, 
        int max_width, 
        int max_height) {
        switch (size) {
            case SMALL:
                min_width = max_width/4;
                min_height = max_height/4;
                break;
            case LARGE:
                min_width = (2*max_width)/3;
                min_height = (2*max_height)/3;
                break; 
            case FULL:
                min_width = max_width;
                min_height = max_height;
                break;
            default:
                std::cout << "ERROR anchors/anchor.h ScreenSize setMinWidthHeight() size invalid\n";
                min_width = -1;
                min_height = -1;
        }

        std::cerr << "screensize setminwidthheight min_width:" << min_width << " min_height:" << min_height << " max_width:" << max_width << " max_height:" << max_height << "\n";
    }

    bool isFull() {
        return size == FULL;
    }

};

class Anchor { // interface via abstract class
    public:
    std::string name;
    int width = 0, height = 0, ws, hs, min_ws, min_hs;
    int gap, xs[11], ys[11], dx, dy;
    double palmbase_x, palmbase_y;
    double indexbase_x, indexbase_y;

    double momentum;
    cv::Scalar color_red, color_blue, color_green, color_cur;

    int selected_i, selected_j, selected_i_prv, selected_j_prv; 
    int green_i, green_j;
    std::string message, message_selected;

    bool static_display;

    int divisions;

    int progress_maxwidth, progress_maxheight; // progress bar
    int pwidth, npwidth;

    ScreenSize screen;

    virtual ~Anchor();

    void setConfig(int _width, int _height);

    // pure virtual functions
    virtual std::string type() = 0;

    virtual void calculate(
        const cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        ExtraParameters & params) = 0;
    
    virtual void draw(
        cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        const ExtraParameters & params) = 0;

    std::tuple<int, int> selectedIndexes();

    void highlightSelected();
    void setDivisions(int _divisions);
    int getDivisions();
    void drawProgressBar(cv::Mat & _image, double _progress);

    void setupGrid(double enlarged_topleft_x, double enlarged_topleft_y);
    void setupSelection(int index_pointer_x, int index_pointer_y);
    void drawTextHighlighted(cv::Mat & overlay);
    void drawTextSelected(cv::Mat & overlay);
    
    void reset_palmbase();
    void reset_indexbase();
    void reset_grid();

    cv::Rect getGrid();
    cv::Point getGridTopLeft();
    cv::Point getGridBottomRight();

    void setScreenSize(eScreenSize size);
};


class AnchorDynamic: public Anchor {
    double palmbase_x_prv, palmbase_y_prv;
    double indexbase_x_prv, indexbase_y_prv;

    public:
    ~AnchorDynamic();
    AnchorDynamic();
    AnchorDynamic(cv::Scalar red, cv::Scalar blue);
    std::string type();
    
    void calculate(
        const cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        ExtraParameters & params);

    void draw(
        cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double area, 
        int pointer_x, int pointer_y,
        const ExtraParameters & params);

    void updatePalmBase(const std::tuple<double, double, double> & palmbase);
    void updateIndexBase(const std::tuple<double, double, double> & indexbase);
    void initiate();
};


class AnchorStatic: public Anchor {
    public:
    ~AnchorStatic();
    AnchorStatic();
    AnchorStatic(cv::Scalar red, cv::Scalar blue, std::string imagePath);
    std::string type();

    cv::Mat image_palm, mask;

    int palm_ubx, palm_uby;
    int palmstart_x, palmstart_y;

    void calculate(
        const cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        ExtraParameters & params);

    void draw(
        cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        const ExtraParameters & params);
    
    void initiate();
    void setup_palmiamge(std::string imagePath);
    void checkSelectionWithinPalm(
        int pointer_x, int pointer_y,
        const std::tuple<double, double, double> & palmbase);
};


class AnchorMidAir: public Anchor {
    public:
    ~AnchorMidAir();
    AnchorMidAir();
    AnchorMidAir(cv::Scalar red, cv::Scalar blue);
    std::string type();

    int palmstart_x, palmstart_y;

    void calculate(
        const cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        ExtraParameters & params);

    void draw(
        cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        const ExtraParameters & params);
};


#endif