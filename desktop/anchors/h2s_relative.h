#ifndef H2S_RELATIVE_H
#define H2S_RELATIVE_H



#include "anchor.h"

class AnchorH2SRelative: public Anchor {

    std::string m_image_path;
    cv::Mat m_background;
    cv::Mat image_palm, mask;
    int palm_ubx, palm_uby;
    int m_inputgrid_topleft_x, m_inputgrid_topleft_y;
    double m_image_palm_dim_max;

    struct {
        cv::Point m_topleft, m_bottomright;    
    } m_input_bound;

    
    public:
    ~AnchorH2SRelative();
    AnchorH2SRelative();
    AnchorH2SRelative(
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
        double area, 
        int pointer_x, int pointer_y,
        Parameters & params);

    void updateBase(const SmoothCoord & base, double & _x, double & _y);
    void initiate();
    void setup_palmiamge(std::string imagePath, int x_cols, int y_rows);
    void setupInputGrid(
        int cursor_x, 
        int cursor_y,
        const std::pair<double, double> & base
    );
};


#endif