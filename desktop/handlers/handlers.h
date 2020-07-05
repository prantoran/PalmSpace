#ifndef HANDLERS_H
#define HANDLERS_H


#include "mediapipe/framework/port/status.h"
#include "desktop/anchors/anchors.h"
#include "desktop/triggers/triggers.h"
#include "desktop/initiators/initiators.h"

#include <tuple>
#include <vector>

class AnchorHandler{
    public:
    
    AnchorDynamic _dynamic;
    AnchorStatic _static;
    int _choice;

    cv::Mat transform(
        const cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double area, 
        int pointer_x, int pointer_y,
        const std::vector<double> & extra_params);
    
    void reset_palmbase();

    std::tuple<int, int> selectedIndexes();
    void setGreen(int i, int j);
    bool static_display();
};

class TriggerHandler {
    public:

    TriggerThumb _thumb;
    TriggerThumbOther _thumb_other;
    TriggerPinch _pinch;
    TriggerWait _wait;
    
    
    int _choice;

    void update(const std::vector<std::vector<std::tuple<double, double, double>>> & points);
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
};


::mediapipe::Status RunMPPGraph(
    InitiatorHandler& initiator,
    AnchorHandler& anchor, 
    TriggerHandler& trigger, 
    const std::string& calculator_graph_config_file,
    const std::string& input_video_path,
    const std::string& output_video_path,
    const int frame_width,
    const int frame_height,
    const int fps,
    const int debug_mode);




#endif