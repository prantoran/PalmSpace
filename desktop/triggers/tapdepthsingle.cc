#include "triggers.h"


TriggerTapDepthSingle::TriggerTapDepthSingle() {
    m_depth = -1;
    positive_frames_cnt = 0;
    cur_state = TRIGGER::OPEN;
}

void TriggerTapDepthSingle::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    ExtraParameters & params) {
    
    m_depth_prev = m_depth;
    // median_depth_region(params, depth_cursor);        
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

    std::cerr << "m_depth:" << m_depth << "\tm_depth_prev:" << m_depth_prev << "\tm_buffer:" << m_buffer << "\n";

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
    ExtraParameters & params,
    double & depth_cursor) {

    // indextop = 8
    // base = palm = 0, indexbottom = 5

    depth_cursor = 0;

    double dx = params.m_frame_width * (std::get<0>(points[0][8]) - std::get<0>(points[0][5]));
    double dy = params.m_frame_height * (std::get<1>(points[0][8]) - std::get<1>(points[0][5]));
    
    double d2 = (dx*dx) + (dy*dy);

    depth_cursor = d2;



}


void TriggerTapDepthSingle::median_depth_region(
    ExtraParameters & params, 
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