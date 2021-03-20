#ifndef HANDTOSCREEN_H
#define HANDTOSCREEN_H


#include "anchor.h"


class AnchoHandToScreen: public Anchor {
    public:
    cv::Mat image_palm, mask;
    double image_palm_dim_max;

    int palm_ubx, palm_uby;
    cv::Mat m_background;
    // SmoothCoord m_indexbase;
    
    int m_inputgrid_topleft_x, m_inputgrid_topleft_y;

    struct {
        cv::Point m_topleft, m_bottomright;    
    } m_input_bound;

    std::string m_image_path;
        
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
    void setup_palmiamge(std::string imagePath, int x_cols, int y_rows);
    
    void setupInputGrid(
        int cursor_x, 
        int cursor_y,
        const SmoothCoord & base);
    
    void align_grid(Grid & _grid, const SmoothCoord & _coord);

    bool isInside(int cursor_x, int cursor_y) const;
};


#endif