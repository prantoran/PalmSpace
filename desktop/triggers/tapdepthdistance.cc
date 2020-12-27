#include "triggers.h"


TriggerTapDepthDistance::TriggerTapDepthDistance() {
    m_depth = -1;
    positive_frames_cnt = 0;
    cur_state = TRIGGER::OPEN;
}

void TriggerTapDepthDistance::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {
    
    m_depth_prev = m_depth;
    median_depth_region(params, m_depth);        
    // finger_length_ratio(points, params, m_depth);
    if (m_depth_prev > -1) {
        m_depth = (0.7*m_depth) + (0.3*m_depth_prev);
    }

    params.m_show_depth_txt = true;
    params.m_depth_txt = std::to_string(m_depth);
    std::cerr << "m_depth:" << m_depth << "\tpositive_frames_cnt:" << positive_frames_cnt << "\tcur_state:" << cur_state << "\n";
}


void TriggerTapDepthDistance::median_depth_region(
    Parameters & params, 
    double & depth_cursor) {

    int cursor_midfinger_x_col, cursor_midfinger_y_col;    
    params.get_primary_cursor_middlefinger_base_cv_indices(
        cursor_midfinger_x_col, cursor_midfinger_y_col);

    m_id = 0;

    for (int i = -5, y_row; i < 6; i ++) {
        y_row = cursor_midfinger_y_col + i;
        if (y_row < 0 || y_row >= params.m_frame_height) continue;
        for (int j = -5, x_col; j < 6; j ++) {
            x_col = cursor_midfinger_x_col + j;
            if (x_col <= 0 || x_col >= params.m_frame_width) continue;
            float _d = params.get_depth(x_col, y_row);
            if (_d > 0.f && _d < 2) {
                // std::cerr << "\tx_col:" << x_col << "\ty_row:" << y_row << "\tdepth:" << _d << "\n";
                m_median_depths[m_id] = _d;
                m_id ++;
            }
        }
    }

    if (!m_id) {
        depth_cursor = m_depth_prev;
    } else {
        std::sort(m_median_depths, m_median_depths + m_id);        
        depth_cursor = m_median_depths[m_id/2];
        m_depth_prev = depth_cursor;
    }
}