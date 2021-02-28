


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
#include "desktop/ui/ui.h"

constexpr double alpha = 0.4;
#define COLORS_floralwhite  cv::Scalar(240,250,255)

// forward declaration, defined in config/config.h
// class Parameters;

class ScreenSize {
    public:
    choices::eScreenSize size;
    
    void setMinWidthHeight(
        double &min_width, 
        double &min_height, 
        int max_width, 
        int max_height) {
        switch (size) {
            case choices::SMALL:
                min_width = max_width/5;
                min_height = max_height/5;
                break;
            case choices::LARGE:
                min_width = (1*max_width)/4;
                min_height = (1*max_height)/4;
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
    double m_progress_bar;

    public:
    choices::anchor::types m_type; 
    std::string name;
    int width = 0, height = 0;
    Grid m_grid, m_grid_out;

    double palmbase_x, palmbase_y;
    double indexbase_x, indexbase_y;

    cv::Scalar color_red, color_blue, color_green, color_cur;

    int m_selected_i, m_selected_j, m_selected_i_prv, m_selected_j_prv; 

    // TODO 2020/11/07: refactor green to marked
    int green_i, green_j;
    
    std::string message, message_selected;

    bool m_static_display; // if true then keep on showing display

    int progress_maxwidth, progress_maxheight; // progress bar
    int pwidth, npwidth;

    ScreenSize m_screen;

    bool m_selection_locked;

    Anchor();
    virtual ~Anchor();

    void setConfig(int _width, int _height);

    // pure virtual functions
    virtual void calculate(
        const cv::Mat& input,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params) = 0;
    
    virtual void draw(
        const cv::Mat& input, 
        cv::Mat& output,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params) = 0;

    std::tuple<int, int> selectedIndexes();

    void highlightSelected();
    void setDivisions(int _divisions);
    int getDivisions();
    void drawProgressBar(cv::Mat & _image, Parameters & params);

    void setupGrid(double enlarged_topleft_x, double enlarged_topleft_y);
    void setupSelection(int index_pointer_x, int index_pointer_y, 
        int & selected_row_i, int & selected_col_j);
    
    void drawTextHighlighted(cv::Mat & overlay);
    void drawTextSelected(cv::Mat & overlay);
    
    void reset_palmbase();
    void reset_indexbase();
    void reset_grids();

    void setScreenSize(const choices::eScreenSize & size);

    void setVisibility(const choices::eVisibility & _visibility);
    choices::eVisibility getVisibility();
    bool isVisible(const Parameters & params);

    choices::anchor::types type();

    void lock_selection();
    void unlock_selection();
    void draw_main_grid_layout(cv::Mat & src, const Grid & grid);
    void draw_cells(cv::Mat & src, const Grid & grid);
};


class AnchorDynamic: public Anchor {
    public:
    ~AnchorDynamic();
    AnchorDynamic();
    AnchorDynamic(
        const cv::Scalar & red, 
        const cv::Scalar & blue);
        
    void calculate(
        const cv::Mat& input,
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params);

    void draw(
        const cv::Mat& input, 
        cv::Mat& output,  
        double area, 
        int pointer_x, int pointer_y,
        Parameters & params);

    void updateBase(const SmoothCoord & base, double & _x, double & _y);
    void initiate();
};


class AnchorStatic: public Anchor {
    public:
    cv::Mat image_palm, mask;

    int palm_ubx, palm_uby;
    int palmstart_x, palmstart_y;

    ~AnchorStatic();
    AnchorStatic();
    AnchorStatic(
        const cv::Scalar & red, 
        const cv::Scalar & blue, 
        const std::string & imagePath);


    void calculate(
        const cv::Mat& input,   
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params);

    void draw(
        const cv::Mat& input, 
        cv::Mat& output,
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params);
    
    void initiate();
    void setup_palmiamge(std::string imagePath);
    void checkSelectionWithinPalm(
        int pointer_x, int pointer_y,
        const SmoothCoord & palmbase);
    
    void ensureMarkedCellWithinPalm(int & marked_row_i, int & marked_col_j);
    
};


class AnchoHandToScreen: public Anchor {
    public:
    cv::Mat image_palm, mask;

    int palm_ubx, palm_uby;
    int palmstart_x, palmstart_y;

    ~AnchoHandToScreen();
    AnchoHandToScreen();
    AnchoHandToScreen(
        const cv::Scalar & red, 
        const cv::Scalar & blue, 
        const std::string & imagePath);


    void calculate(
        const cv::Mat& input, 
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params);

    void draw(
        const cv::Mat& input, 
        cv::Mat& output,
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params);
    
    void initiate();
    void setup_palmiamge(std::string imagePath);
    void checkSelectionWithinPalm(
        int pointer_x, int pointer_y,
        const SmoothCoord & palmbase);
    
    void ensureMarkedCellWithinPalm(int & marked_row_i, int & marked_col_j);
    
};


class AnchorPad: public Anchor {
    public:
    cv::Mat image_palm, mask;

    int palm_ubx, palm_uby;
    int palmstart_x, palmstart_y;
    std::pair<double, double> m_palm_x, m_palm_y;
    
    ~AnchorPad();
    AnchorPad();
    AnchorPad(
        const cv::Scalar & red, 
        const cv::Scalar & blue, 
        const std::string & imagePath);

    void calculate(
        const cv::Mat& input,   
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params);

    void draw(
        const cv::Mat& input, 
        cv::Mat& output,
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params);
    
    void initiate();
    void setup_palmiamge(std::string imagePath);
    void checkSelectionWithinPalm(
        int pointer_x, int pointer_y,
        const SmoothCoord & palmbase);
    
    void ensureMarkedCellWithinPalm(int & marked_row_i, int & marked_col_j);
    
};


class AnchorPadLarge: public Anchor {
    public:
    cv::Mat image_palm, mask;

    int palm_ubx, palm_uby;
    int palmstart_x, palmstart_y;
    std::pair<double, double> m_palm_x, m_palm_y;
    SmoothCoord m_indexbase;
    cv::Mat m_background;

    ~AnchorPadLarge();
    AnchorPadLarge();
    AnchorPadLarge(
        const int _width,
        const int _height,
        const cv::Scalar & red, 
        const cv::Scalar & blue, 
        const std::string & imagePath,
        const std::string & imagePathBackground);

    void calculate(
        const cv::Mat& input,   
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params);
 
    void draw(
        const cv::Mat& input, 
        cv::Mat& output,
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params);
    
    void initiate();
    void setup_palmiamge(std::string imagePath, int _width, int _height);    
    void setup_background(std::string _imagePath, int _width, int _height);
};

#endif