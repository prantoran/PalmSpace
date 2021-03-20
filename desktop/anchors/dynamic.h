#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "anchor.h"

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



#endif