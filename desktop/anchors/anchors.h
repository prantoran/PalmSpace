


#ifndef ANCHORS_H
#define ANCHORS_H
#include <string>
#include <tuple>
#include <vector>
#include <iostream>

#include "mediapipe/framework/port/opencv_imgproc_inc.h" // most likely contains headers for cv::types
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

#include "desktop/config/colors.h"
#include "desktop/config/config.h"
#include "desktop/config/choices.h"

constexpr double alpha = 0.4;
#define COLORS_floralwhite  cv::Scalar(240,250,255)

// forward declaration, defined in config/config.h
// class ExtraParameters;

class ScreenSize {
    public:
    choices::eScreenSize size;
    
    void setMinWidthHeight(
        int &min_width, 
        int &min_height, 
        int max_width, 
        int max_height) {
        switch (size) {
            case choices::SMALL:
                min_width = max_width/3;
                min_height = max_height/3;
                break;
            case choices::LARGE:
                min_width = (1*max_width)/2;
                min_height = (1*max_height)/2;
                break; 
            case choices::FULL:
                min_width = max_width;
                min_height = max_height;
                break;
            default:
                std::cout << "ERROR anchors/anchor.h ScreenSize setMinWidthHeight() size invalid\n";
                min_width = -1;
                min_height = -1;
        }

    }

    bool isFull() {
        return size == choices::FULL;
    }

    bool isCentered() {
        // TODO 2020/11/07: get choice from UI
        return true;
    }

};

class Anchor { // interface via abstract class
    choices::eVisibility visibility;
    
    public:
    choices::anchor::types _type;
    std::string name;
    int width = 0, height = 0, ws, hs, min_ws, min_hs;
    int gap, xs[11], ys[11], dx, dy;
    double palmbase_x, palmbase_y;
    double indexbase_x, indexbase_y;

    double momentum;
    cv::Scalar color_red, color_blue, color_green, color_cur;

    int selected_i, selected_j, selected_i_prv, selected_j_prv; 

    // TODO 2020/11/07: refactor green to marked
    int green_i, green_j;
    
    std::string message, message_selected;

    bool static_display; // if true then keep on showing display

    int divisions;

    int progress_maxwidth, progress_maxheight; // progress bar
    int pwidth, npwidth;

    ScreenSize screen;


    virtual ~Anchor();

    void setConfig(int _width, int _height);

    // pure virtual functions
    virtual void calculate(
        const cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        ExtraParameters & params) = 0;
    
    virtual void draw(
        const cv::Mat& input, 
        cv::Mat& output,  
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
    void setupSelection(int index_pointer_x, int index_pointer_y, 
        int & selected_row_i, int & selected_col_j);
    
    void drawTextHighlighted(cv::Mat & overlay);
    void drawTextSelected(cv::Mat & overlay);
    
    void reset_palmbase();
    void reset_indexbase();
    void reset_grid();

    cv::Rect getGrid();
    cv::Point getGridTopLeft();
    cv::Point getGridBottomRight();

    void setScreenSize(const choices::eScreenSize & size);

    void setVisibility(const choices::eVisibility & _visibility);
    choices::eVisibility getVisibility();
    bool isVisible(const ExtraParameters & params);

    choices::anchor::types type();
};


class AnchorDynamic: public Anchor {
    double palmbase_x_prv, palmbase_y_prv;
    double indexbase_x_prv, indexbase_y_prv;

    public:
    ~AnchorDynamic();
    AnchorDynamic();
    AnchorDynamic(
        const cv::Scalar & red, 
        const cv::Scalar & blue);
        
    void calculate(
        const cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        ExtraParameters & params);

    void draw(
        const cv::Mat& input, 
        cv::Mat& output, 
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
    AnchorStatic(
        const cv::Scalar & red, 
        const cv::Scalar & blue, 
        const std::string & imagePath);

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
        const cv::Mat& input, 
        cv::Mat& output, 
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
    
    void ensureMarkedCellWithinPalm(int & marked_row_i, int & marked_col_j);
    
};


class AnchorMidAir: public Anchor {
    public:
    ~AnchorMidAir();
    AnchorMidAir();
    AnchorMidAir(
        const cv::Scalar & red, 
        const cv::Scalar & blue);
    
    int palmstart_x, palmstart_y;

    void calculate(
        const cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        ExtraParameters & params);

    void draw(
        const cv::Mat& input, 
        cv::Mat& output, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        const ExtraParameters & params);
    
    void initiate();
};


#endif