#include "triggers.h"


TriggerTapDepthSingle::TriggerTapDepthSingle() {
    m_depth = -1;
    positive_frames_cnt = 0;
    cur_state = TRIGGER::OPEN;
}

void TriggerTapDepthSingle::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {
    
    m_depth_prev = m_depth;
    finger_length_ratio(points, params, m_depth);
    if (m_depth_prev > -1) {
        m_depth = (0.7*m_depth) + (0.3*m_depth_prev);
    }

    if (m_depth - m_depth_prev > 800.0f) {
        m_buffer = 0;
        positive_frames_cnt ++;
        if (positive_frames_cnt > 3) 
            positive_frames_cnt = 3;
    } else if (m_depth -m_depth_prev < -500.0f) {
        positive_frames_cnt --;
        if (positive_frames_cnt < 0) 
            positive_frames_cnt = 0;
    } else {
        m_buffer ++;
    }

    std::cerr << "finger ratio m_depth:" << m_depth << "\tm_depth_prev:" << m_depth_prev << "\tm_buffer:" << m_buffer << "\n";

    if (m_buffer > 5) {
        // reseting
        cur_state = TRIGGER::OPEN;
        m_buffer = 0;
        positive_frames_cnt = 0;
    }


    if (positive_frames_cnt > 1) {
        if (cur_state == TRIGGER::OPEN) {
            cur_state = TRIGGER::PRESSED;
        }
    } else if (positive_frames_cnt < 1) {
        if (cur_state == TRIGGER::PRESSED) {
            cur_state = TRIGGER::RELEASED;
        } else if (cur_state == TRIGGER::RELEASED) {
            positive_frames_cnt = 0;
            cur_state = TRIGGER::OPEN;
        }
    } else {
        if (cur_state == TRIGGER::RELEASED) {
            cur_state = TRIGGER::OPEN;
        }
    }

    std::cerr << "m_depth:" << m_depth << "\tpositive_frames_cnt:" << positive_frames_cnt << "\tcur_state:" << cur_state << "\n";


}


void TriggerTapDepthSingle::finger_length_ratio(
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params,
    double & depth_cursor) {

    // indextop = 8
    // base = palm = 0, indexbottom = 5

    depth_cursor = 0;

    double dx = params.m_frame_width * (std::get<0>(points[params.m_base_id][8]) - std::get<0>(points[params.m_base_id][5]));
    double dy = params.m_frame_height * (std::get<1>(points[params.m_base_id][8]) - std::get<1>(points[params.m_base_id][5]));
    
    double d2 = (dx*dx) + (dy*dy);

    depth_cursor = d2;



}
