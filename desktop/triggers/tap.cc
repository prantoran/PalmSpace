#include "triggers.h"


TriggerTap::TriggerTap() {
    cur_state = TRIGGER::OPEN;
    m_cnt = 0;
    m_base_rel_depth = -1;
    m_diff = 0;
}


void TriggerTap::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {
    
    const int & rel_depth = params.m_hand_size_scale[params.cursor_hand_id()];
    if (m_base_rel_depth == -1) m_base_rel_depth = rel_depth; 
    m_diff = rel_depth - m_base_rel_depth;

    std::cerr << "trigger/tap size:" << rel_depth << "\tm_diff:" << m_diff << "\n";
    if (m_diff < -2) {
        if (cur_state != TRIGGER::PRESSED) {
            cur_state = TRIGGER::PRESSED;
            m_cnt = 0;
        } else {
            m_cnt ++;
        }
    } else if (m_diff > 2) {
        if (cur_state == TRIGGER::PRESSED) {
            cur_state = TRIGGER::RELEASED;
            m_cnt = 0;
        } else {
            m_cnt ++;
        }
    } else {
        m_cnt ++;
    }

    m_base_rel_depth = 0.2*m_base_rel_depth + 0.8*rel_depth;

    if (m_cnt > 10) {
        m_cnt = 0;
        cur_state = TRIGGER::OPEN;
    } 
}
