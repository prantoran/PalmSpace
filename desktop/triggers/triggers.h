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
        ExtraParameters & params) = 0;

    // TODO: refactor status def to parent
    virtual TRIGGER::state status() = 0;
};

class TriggerThumb: public Trigger {
    public:

    TriggerThumb();
    TriggerThumb(int _width, int _height);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        ExtraParameters & params);

    TRIGGER::state status();
};


class TriggerThumbOther: public Trigger {
    public:

    TriggerThumbOther();
    TriggerThumbOther(int _width, int _height);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        ExtraParameters & params);

    TRIGGER::state status();
};

class TriggerPinch: public Trigger {
    public:

    TriggerPinch();
    TriggerPinch(int _width, int _height);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        ExtraParameters & params);

    TRIGGER::state status();
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
        ExtraParameters & params);

    TRIGGER::state status();
};

class TriggerTap: public Trigger {
    public:

    int cnt;
    double zvalue, prev_zvalue;
    int hand_ID, prev_handID, hand_switch_ignore_cnt;
    
    TriggerTap();
    TriggerTap(int _width, int _height);

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        ExtraParameters & params);

    TRIGGER::state status();
};

class TriggerTapPalm: public Trigger {

    public:

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
        ExtraParameters & params);

    TRIGGER::state status();
};


class TriggerDwell: public Trigger {
    std::chrono::milliseconds _timestamp[11][11], ctime, ptime;
    int selected_i, selected_j, selected_i_prv, selected_j_prv; 
        
    public:

    TriggerDwell();

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        ExtraParameters & params);

    TRIGGER::state status();
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
        const int opt_dev_video, 
        const int fps,
        const int frame_width,
        const int frame_height
    );
    
    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        ExtraParameters & params);

    TRIGGER::state status();
    
};


#endif
