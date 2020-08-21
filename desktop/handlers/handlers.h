#ifndef HANDLERS_H
#define HANDLERS_H


#include "mediapipe/framework/port/status.h"
#include "desktop/anchors/anchors.h"
#include "desktop/triggers/triggers.h"
#include "desktop/initiators/initiators.h"

// #include "mediapipe/framework/calculator_framework.h"



#include <tuple>
#include <vector>
#include <queue>

class AnchorHandler{
    public:
    
    AnchorDynamic _dynamic;
    AnchorStatic _static;
    AnchorMidAir _midair;

    int _choice;

    void calculate(
        const cv::Mat& input, 
        double palmbase_x_new, double palmbase_y_new, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params); 

    void draw(
        cv::Mat& input, 
        double palmbase_x_new, double palmbase_y_new, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params);
    
    void reset_palmbase();

    std::tuple<int, int> selectedIndexes();
    void highlightSelected();
    int getDivisions();
    bool static_display();
    void setDivisions(int _divisions);
};

class TriggerHandler {
    public:

    TriggerThumb _thumb;
    TriggerThumbOther _thumb_other;
    TriggerPinch _pinch;
    TriggerWait _wait;
    TriggerTap _tap;
    TriggerTapPalm _tappalm;
    TriggerDwell _dwell;
    
    int _choice;

    void update(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        std::vector<double> & extra_params);
        
    TRIGGER::state status();
};


class InitiatorHandler {
    public:
    InitiatorDefault _default;
    InitiatorTwoHand _twohand;

    int _choice;

    bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points);
    std::vector<double> & params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points);
    void setStrict(bool _strict);
};



class MediaPipeMultiHandGPU {
    public:
    AnchorHandler anchor;
    TriggerHandler trigger;
    InitiatorHandler initiator;
    // int curImageID;

    MediaPipeMultiHandGPU();

    ::mediapipe::Status run(
        const std::string& calculator_graph_config_file,
        const std::string& input_video_path,
        const std::string& output_video_path,
        const int frame_width,
        const int frame_height,
        const int fps,
        const int debug_mode); //,
        // const std::string& image_ext,
        // const unsigned int readDelayMS,
        // std::vector<std::queue<std::shared_ptr<cv::Mat>>> & inq,
        // std::vector<std::queue<std::shared_ptr<cv::Mat>>> & outq);
};




#endif