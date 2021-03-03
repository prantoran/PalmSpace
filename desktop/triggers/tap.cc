#include "tap.h"


TriggerTap::TriggerTap() {
    cur_state = TRIGGER::OPEN;
    m_cnt = 0;
    m_cnt_positive = 0;
    m_base_rel_depth = -1;
    m_diff = 0;
    m_debug = true;
}


void TriggerTap::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {
    
    const int & rel_depth = params.m_hand_size_scale[params.cursor_hand_id()];
    if (m_base_rel_depth == -1) m_base_rel_depth = rel_depth; 
    m_diff = rel_depth - m_base_rel_depth;

    if (m_debug) {
        std::cerr << "trigger/tap size:" << rel_depth << "\tm_diff:" << m_diff << "\n";
    }
    
    if (m_diff < -1) {
        if (cur_state != TRIGGER::PRESSED) {
            m_cnt_positive ++;
            if (m_cnt_positive > 2) {
                cur_state = TRIGGER::PRESSED;
                m_cnt = 0;
                m_cnt_positive = 0;
            } 
        } else if (cur_state == TRIGGER::PRESSED) {
            m_cnt ++;
        }
    } else if (m_diff > 1) {
        if (cur_state == TRIGGER::PRESSED) {
            m_cnt_positive ++;

            if (m_cnt_positive > 2) {
                cur_state = TRIGGER::RELEASED;
                m_cnt = 0;
                m_cnt_positive = 0;
            }
        
        } else if (cur_state == TRIGGER::RELEASED) {
            m_cnt ++;
        }
    } else {
        m_cnt ++;
        m_cnt_positive = 0;
    }

    m_base_rel_depth = 0.2*m_base_rel_depth + 0.8*rel_depth;

    if (m_cnt > 3) {
        m_cnt = 0;
        m_cnt_positive = 0;
        cur_state = TRIGGER::OPEN;
    } 
}
