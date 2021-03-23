


#ifndef ANCHORS_H
#define ANCHORS_H
#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include <utility>

#include "desktop/config/choices.h"

#include "anchor.h"
#include "handtoscreen.h"
#include "dynamic.h"
#include "s2h_absolute.h"
#include "h2s_relative.h"

// forward declaration, defined in config/config.h
// class Parameters;


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