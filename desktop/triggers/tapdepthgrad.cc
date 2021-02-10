#include "triggers.h"

#include <queue>
#include <utility>
#include <unordered_map>


TriggerTapDepthGradient::TriggerTapDepthGradient() {
    m_depth_cursor = -1;

    m_cursor_pts.init(1);

    for (int k = 0, i = 9; i <=9; i+=4, k ++) {
        m_cursor_indices[k] = i;
    }

    // channel medians start
    m_channel_cnt = 3;
    // end

    cur_state = TRIGGER::OPEN;

    // init series for pixel value chanages
    m_px_window_sz = 30;
    m_px_window_i = 0;
    m_cyclic_pixels_window = std::vector<std::vector<int>>(m_channel_cnt);
    for (int i = 0; i < m_channel_cnt; i ++) {
        m_cyclic_pixels_window[i] = std::vector<int>(m_px_window_sz);
    }

    std::memset(m_ticks, 0, sizeof(m_ticks));

    m_max_axis_spatial_diff_thresh_exceed_cnt = 0;
    m_stable_frames_cnt = 0;
}


void TriggerTapDepthGradient::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {
    
    std::cerr << "tapdepthgrad update()\n";

    m_cursor_pts.backup_prev();
    std::cerr << "m_cursor_pts backuped\n";

    m_cursor_pts.update_support(m_cursor_indices, points[1-params.m_base_id], params);

    std::cerr << "m_cursor_pts updated\n";

    m_cursor_pts.max_diff(m_max_axis_spatial_diff);
    std::cerr << "m_max_axis_spatial_diff:" << m_max_axis_spatial_diff << "\n";
    if (m_max_axis_spatial_diff > 20) {
        m_max_axis_spatial_diff_thresh_exceed_cnt ++;
        if (m_max_axis_spatial_diff_thresh_exceed_cnt > 3) {
            m_max_axis_spatial_diff_thresh_exceed_cnt = 3;
        }
    } else {
        m_max_axis_spatial_diff_thresh_exceed_cnt --;
        if (m_max_axis_spatial_diff_thresh_exceed_cnt < 0)
            m_max_axis_spatial_diff_thresh_exceed_cnt = 0;
    }

    if (m_max_axis_spatial_diff_thresh_exceed_cnt > 1) {
        cur_state = TRIGGER::OPEN;
        return;
    }

    if (m_max_axis_spatial_diff < 10) {
        m_stable_frames_cnt ++;
    }else {
        m_stable_frames_cnt = 0;
    }
    
    std::cerr << "m_stable_frames_cnt: " << m_stable_frames_cnt << "\n"; 

    for (int i = 0; i < m_channel_cnt; i ++) {
        m_ch_medians[i] = 0;
    }

    std::cerr << "calculating cursor medians\n";
    for (int i = 0; i < m_cursor_pts.m_support_pts_size; i ++) {
        get_channel_medians(
            params.depth_mat, 
            m_cursor_pts.m_xcoords[i], 
            m_cursor_pts.m_ycoords[i]);

    }


    for (int i = 0; i < m_channel_cnt; i ++) {
        m_ch_medians[i] =  m_ch_medians[i]/m_channel_cnt + ((m_ch_medians[i]%m_channel_cnt) > 0);
        
    }

    std::cerr << "channel medians:";
    for (int i = 0; i < m_channel_cnt; i ++) {
        std::cerr << " " << m_ch_medians[i];
    }
    std::cerr << "\n";
    
    for (int i = 0; i < m_channel_cnt; i ++) {
        m_cyclic_pixels_window[i][m_px_window_i] = m_ch_medians[i];
    }

    std::memset(m_ticks, 0, sizeof(m_ticks));

    for (int ti = 0; ti < 2; ti ++) {
        std::cerr << "ti:" << ti << " diffs:";
        for (int i = 15*ti, idx, pdx, diff; i < 15*(ti+1); i ++) {
            idx = (m_px_window_i + i + 1) % m_px_window_sz;
            pdx = (m_px_window_i + i) % m_px_window_sz;

            diff = m_cyclic_pixels_window[1][idx] - m_cyclic_pixels_window[1][pdx]; 
            std::cerr << "\t" << diff;
            if (diff > 10) {
                m_ticks[ti][0] ++;
            } else if (diff < -10) {
                m_ticks[ti][1] ++;
            }
        }

        std::cerr << "\n";
    }

    m_px_window_i = (m_px_window_i+1) % m_px_window_sz;

    m_gesture_pat_found = false;
    m_gesture_pat_initiated = false;

    if (m_ticks[0][0] >= 1) {
        m_gesture_pat_initiated = true;
        if (m_ticks[1][1] >= 1) {
            m_gesture_pat_found = true;
        }
    } else if (m_ticks[0][1] >= 1) {
        m_gesture_pat_initiated = true;
        if (m_ticks[1][0] >= 1) {
            m_gesture_pat_found = true;
        }
    }

    std::cerr << "(" << m_ticks[0][0] << ", " << m_ticks[1][1] << ") (" << m_ticks[0][1] << ", " << m_ticks[1][0] << ")\n";
    

    cur_state = TRIGGER::OPEN;
    if (m_gesture_pat_initiated && m_stable_frames_cnt > 6) {
        cur_state = TRIGGER::PRESSED;
    }
    
    if (cur_state == TRIGGER::PRESSED && m_gesture_pat_found) {
        cur_state = TRIGGER::RELEASED;
        m_stable_frames_cnt = 0;
    } 

    if (cur_state == TRIGGER::OPEN) std::cerr << "cur_state: OPEN\n";
    else if (cur_state == TRIGGER::PRESSED) std::cerr << "curstate: PRESSED\n";
    else if (cur_state == TRIGGER::RELEASED) std::cerr << "cur_state: RELEASD\n";

}


void TriggerTapDepthGradient::get_channel_medians(cv::Mat * mat, int col_x, int row_y) {
    for (int i = 0; i < m_channel_cnt; i ++) {
        m_median_indices[i] = 0;
    }

    for (int i = row_y-2; i < row_y+3; i ++) {
        for (int j = col_x-2; j < col_x+3; j ++) {
            cv::Vec3b u = (*mat).at<cv::Vec3b>(i, j);
            
            for (int k = 0;k < m_channel_cnt; k ++) {
                int pixel_value = u[k];
                if (!pixel_value) continue;
                m_medians[k][m_median_indices[k]] = pixel_value;
                m_median_indices[k] ++;
            }
        }
    }

    for (int i = 0; i < m_channel_cnt; i ++) {
        std::sort(m_medians[i], m_medians[i]+m_median_indices[i]);
    }

    for (int i = 0; i < m_channel_cnt; i ++) {
        m_ch_medians[i] += m_medians[i][m_median_indices[i]/2];
    }
}
