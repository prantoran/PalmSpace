


#ifndef ANCHORS_H
#define ANCHORS_H
#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include <utility>

#include "desktop/config/choices.h"

#include "anchor.h"

constexpr double TRANSPARENCY_ALPHA = 0.4;

// forward declaration, defined in config/config.h
// class Parameters;


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
    cv::Mat m_background;
    // SmoothCoord m_indexbase;
    
    int m_inputgrid_topleft_x, m_inputgrid_topleft_y;

    struct {
        cv::Point m_topleft, m_bottomright;    
    } m_input_bound;

    ~AnchoHandToScreen();
    AnchoHandToScreen();
    AnchoHandToScreen(
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
    void setup_palmiamge(std::string imagePath);
    
    void setupInputGrid(
        int cursor_x, 
        int cursor_y,
        const SmoothCoord & base);
    
    void align_grid(Grid & _grid, const SmoothCoord & _coord);

    bool isInside(int cursor_x, int cursor_y) const;
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
};

#endif