
#include "triggers.h"

// internal linkage, not visible outside
// for external linkage, use extern
const int index_bottom = 5;
const int thumb_top = 4;

TriggerThumbOther::TriggerThumbOther() {}

TriggerThumbOther::TriggerThumbOther(int _width, int _height) {
    width = _width;
    height = _height;
    cur_state = TRIGGER::OPEN;
    // std::cerr <<"TriggerThumbOther constructor _width:" << _width << " _height:" << _height << " width:" << width << " height:" << height << "\n";
}

void TriggerThumbOther::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    ExtraParameters & params) {

    std::vector<double> & extra_params = params.extra_params;


    if (points[1].size() < 6) {
        std::cerr << "trigger_thumb_other=not enough points for free palm\n";
        return;
    }

    double dx = std::get<0>(points[1][index_bottom]) - std::get<0>(points[1][thumb_top]);
    double dy = std::get<1>(points[1][index_bottom]) - std::get<1>(points[1][thumb_top]);

    dx *= width;
    dy *= height;

    // double dx = std::get<0>(points[0]) - std::get<0>(points[1]);
    // double dy = std::get<1>(points[0]) - std::get<1>(points[1]);

    double d = 10*(dx*dx + dy*dy);

    if (d < 30000) {
        if (cur_state == TRIGGER::OPEN) {
            cur_state = TRIGGER::PRESSED;
        } else if (cur_state == TRIGGER::PRESSED) {
            // do nothing
        } else if (cur_state == TRIGGER::RELEASED) {
            std::cerr << "thumb trigger update state wrong state, previously in released state, but currently pressed\n";
        } else {
            std::cerr << "thumb trigger update state invalid, distance:" << d << "\n";
        }
    } else {
        if (cur_state == TRIGGER::OPEN) {
            // do nothing
        } else if (cur_state == TRIGGER::PRESSED) {
            cur_state = TRIGGER::RELEASED;
            std::cerr << "trigger released\n";
        } else if (cur_state == TRIGGER::RELEASED) {
            cur_state = TRIGGER::OPEN;
        } else {
            std::cerr << "thumb trigger update state invalid, distance:" << d << "\n";
        }
    }

    std::cerr << "distance:" << d << " cur_state:" << cur_state << " width:" << width << " height:" << height << "\n";
}


TRIGGER::state TriggerThumbOther::status() {
    return cur_state;
}