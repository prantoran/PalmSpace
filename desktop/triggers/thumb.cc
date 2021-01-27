
#include "triggers.h"

// internal linkage, not visible outside
// for external linkage, use extern
const int index_bottom = 5;
const int thumb_top = 4;

TriggerThumb::TriggerThumb() {}

TriggerThumb::TriggerThumb(int _width, int _height) {
    width = _width;
    height = _height;
    cur_state = TRIGGER::OPEN;
}

void TriggerThumb::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {

    if (points[params.m_base_id].size() <= index_bottom) {
        std::cout << "trigger thumb update: not enough points\n";
        cur_state = TRIGGER::OPEN;
        return;
    }
    
    
    double dx = std::get<0>(points[params.m_base_id][index_bottom]) - std::get<0>(points[params.m_base_id][thumb_top]);
    double dy = std::get<1>(points[params.m_base_id][index_bottom]) - std::get<1>(points[params.m_base_id][thumb_top]);

    dx *= width;
    dy *= height;

    double d = 10*(dx*dx + dy*dy);

    if (d < 30000) {
        if (cur_state == TRIGGER::OPEN) {
            cur_state = TRIGGER::PRESSED;
        } else if (cur_state == TRIGGER::PRESSED) {
            // do nothing
        } else if (cur_state == TRIGGER::RELEASED) {
            std::cout << "thumb trigger update state wrong state, previously in released state, but currently pressed\n";
        } else {
            std::cout << "thumb trigger update state invalid, distance:" << d << "\n";
        }
    } else {
        if (cur_state == TRIGGER::OPEN) {
            // do nothing
        } else if (cur_state == TRIGGER::PRESSED) {
            cur_state = TRIGGER::RELEASED;
        } else if (cur_state == TRIGGER::RELEASED) {
            cur_state = TRIGGER::OPEN;
        } else {
            std::cout << "thumb trigger update state invalid, distance:" << d << " cur_state:" << state_str() << "\n";
        }
    }
}
