#ifndef TRIGGERS_H
#define TRIGGERS_H


#include <tuple>
#include <vector>
#include <chrono>
#include <string>
#include <iostream>


// opencv
#include "mediapipe/framework/port/opencv_imgproc_inc.h"

#include "desktop/config/config.h"


// TODO remove unused triggers

constexpr char trackbarWindowName[] = "Index Marker Tuning";


namespace TRIGGER {
    typedef enum{
        OPEN,
        PRESSED, 
        RELEASED,
        INVALID
    } state;
}

std::chrono::milliseconds cur_time();

class Trigger {
    public:
    int width, height;
    TRIGGER::state cur_state;

    std::string state_str(); 

    virtual ~Trigger();
    

    virtual void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params) = 0;

    TRIGGER::state status();
};

class TriggerThumb: public Trigger {
    public:

    TriggerThumb();
    TriggerThumb(int _width, int _height);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);

};


class TriggerThumbOther: public Trigger {
    public:

    TriggerThumbOther();
    TriggerThumbOther(int _width, int _height);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);

};

class TriggerPinch: public Trigger {
    public:

    TriggerPinch();
    TriggerPinch(int _width, int _height);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);

};

class TriggerWait: public Trigger {
    std::chrono::milliseconds _timestamp[3][3], ctime, ptime;
    int min_ws, min_hs, ws, hs;
    int gap, xs[4], ys[4], dx, dy;
    double palmbase_momentum;
    double palmbase_x, palmbase_y, palmbase_x_prv, palmbase_y_prv;
    double palmbase_x_new, palmbase_y_new;

    int pointer_x, pointer_y;
    int selected_i, selected_j, selected_i_prv, selected_j_prv; 
        
    public:

    int choice;
    TriggerWait();
    TriggerWait(int _width, int _height, int _anchor_choice);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);

};

class TriggerTap: public Trigger {
    public:
    double m_zvalue;
    int cnt;
    double zvalue, prev_zvalue;
    int hand_ID, prev_handID, hand_switch_ignore_cnt;
    
    TriggerTap();
    TriggerTap(int _width, int _height);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);

};

class TriggerTapPalm: public Trigger {

    public:
    double m_zvalue;

    int cnt;
    double zvalue, prev_zvalue;
    int hand_ID, prev_handID, hand_switch_ignore_cnt;
    
    double cache[9];
    int cache_id;

    double st, mid, nd;

    TriggerTapPalm();
    TriggerTapPalm(int _width, int _height);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);

};


class TriggerDwell: public Trigger {
    std::chrono::milliseconds _timestamp[11][11], ctime, ptime;
    int selected_i, selected_j, selected_i_prv, selected_j_prv; 
        
    public:

    TriggerDwell();

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);

};

class TriggerTapDepthArea: public Trigger {
    public:

    int minR, maxR, minG, maxG, minB, maxB;
    int minR2, maxR2, minG2, maxG2, minB2, maxB2;

    cv::Mat hsv, hsv2;

    TriggerTapDepthArea();

    TriggerTapDepthArea(
        const bool save_video, 
        const bool load_video, 
        const int fps,
        const int frame_width,
        const int frame_height
    );
    
    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);
};


class SupportPoints {
    public:
    int m_support_pts_size;
    std::vector<int> m_xcoords, m_ycoords;
    std::vector<int> m_xcoords_prev, m_ycoords_prev;
    SupportPoints();
    void init(int _size);
    void backup_prev();
    void update_support(
        int indices[],
        const std::vector<std::tuple<double, double, double>> & coords,
        const Parameters & params); 
    void print();
};




class TriggerTapDepth: public Trigger {
    public:
    // TODO clean up unused attributes
    double m_depth_base, m_depth_cursor;
    double m_depth_base_prev, m_depth_cursor_prev;
    
    SupportPoints m_base_pts, m_cursor_pts;

    int m_base_indices[4];
    int m_cursor_indices[4];

    // channel medians - flood fill start
    int m_channel_cnt;
    int m_median_size;
    int m_medians[5][100];
    int m_median_indices[5];
    int m_ch_medians[5];
    int m_floodfill_pixel_variance;
    Range m_flood_width, m_flood_height;
    
    // end

    int positive_frames_cnt;

    TriggerTapDepth();
    
    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);
    
    void get_channel_medians(cv::Mat * mat, int col_x, int row_y);
    void process_depths(Parameters & params);
    void palm_rect_fill(Parameters & params, int & cnt);
};


class TriggerTapDepthSingle: public Trigger {
    public:
    float m_median_depths[120];
    double m_depth, m_depth_prev;
    int m_id;
    int positive_frames_cnt;
    int m_buffer;
    
    TriggerTapDepthSingle();

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);
    void finger_length_ratio(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params,
        double & depth_cursor);   
};


class TriggerTapDepthDistance: public Trigger {
    public:
    float m_median_depths[120];
    double m_depth, m_depth_prev;
    int m_id;
    int positive_frames_cnt;
    int m_buffer;
    
    TriggerTapDepthDistance();

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);
    void median_depth_region(Parameters & params, double & depth_cursor);
};

#endif
