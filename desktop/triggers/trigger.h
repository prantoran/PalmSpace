#ifndef TRIGGER_H 
#define TRIGGER_H

#include <tuple>
#include <vector>
#include <chrono>

#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "desktop/config/config.h"


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
    int m_cnt, m_cnt_positive;
    int width, height;

    // TODO rename to m_cur_state
    TRIGGER::state cur_state;

    std::string state_str(); 

    bool m_debug;

    Trigger();

    virtual ~Trigger();
    

    virtual void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params) = 0;

    TRIGGER::state status();
    void reset_status();
};



#endif