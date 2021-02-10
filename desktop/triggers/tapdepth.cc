#include "triggers.h"

#include <queue>
#include <utility>
#include <unordered_map>


TriggerTapDepth::TriggerTapDepth() {
    m_depth_base = -1;
    m_depth_cursor = -1;

    m_base_pts.init(2);
    m_cursor_pts.init(2);

    for (int k = 0, i = 10; i < 15; i += 4, k ++) {
        m_base_indices[k] = i;
    }

    for (int k = 0, i = 9; i <=13; i+=4, k ++) {
        m_cursor_indices[k] = i;
    }

    // channel medians start
    m_channel_cnt = 3;
    m_floodfill_pixel_variance = 15;
    positive_frames_cnt = 0;
    // end

    cur_state = TRIGGER::OPEN;

    m_mean_cnt = 0;
}


void TriggerTapDepth::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {

    m_base_pts.backup_prev();   
    m_base_pts.update_support(m_base_indices, points[params.m_base_id], params);

    m_cursor_pts.backup_prev();
    m_cursor_pts.update_support(m_cursor_indices, points[1-params.m_base_id], params);

    for (int i = 0; i < m_channel_cnt; i ++) {
        m_ch_medians[i] = 0;
    }

    for (int i = 0; i < m_base_pts.m_support_pts_size; i ++) {
        get_channel_medians(
            params.depth_mat, 
            m_base_pts.m_xcoords[i], 
            m_base_pts.m_ycoords[i]);

    }

    for (int i = 0; i < m_channel_cnt; i ++) {
        m_ch_medians[i] =  m_ch_medians[i]/m_channel_cnt + ((m_ch_medians[i]%m_channel_cnt) > 0);
    }

    
    int cnt = 0;
    // flood_fill(params, cnt);
    palm_rect_fill(params, cnt);
    m_mean_cnt = 0.9*m_mean_cnt + (1-0.9)*cnt;

    if (cnt > (int)(m_mean_cnt + 700)) {
        positive_frames_cnt ++;
        if (positive_frames_cnt > 3) 
            positive_frames_cnt = 3;
    } else {
        positive_frames_cnt --;
        if (positive_frames_cnt < 0) 
            positive_frames_cnt = 0;
    }
    std::cerr << "cnt:" << cnt << "\tpositive_frames_cnt:" << positive_frames_cnt << "\tm_mean_cnt:" << m_mean_cnt << "\n";

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

}


void TriggerTapDepth::palm_rect_fill(Parameters & params, int & cnt) {
    int x_col_palmid, y_row_palmid;
    params.get_palmbase_middle_cv_indices(x_col_palmid, y_row_palmid);
    // std::cerr << "x_col_palmid:" << x_col_palmid << "\ty_row_palmid:" << y_row_palmid << "\n";

    m_flood_width = Range(
        std::max(0, x_col_palmid - 40), 
        std::min(x_col_palmid + 40, params.m_frame_height-1));

    m_flood_height = Range(
        std::max(0, y_row_palmid-40), 
        std::min(y_row_palmid + 40, params.m_frame_height-1));
    
    params.m_flood_height = m_flood_height;
    params.m_flood_width = m_flood_width;

    int cursor_x, cursor_y;
    params.get_primary_cursor_cv_indices(cursor_x, cursor_y);

    int cursor_midfinger_x, cursor_midfinger_y;    
    params.get_primary_cursor_middlefinger_base_cv_indices(
        cursor_midfinger_x, cursor_midfinger_y);

    cnt = 0;

    for (int y_row = m_flood_height.m_lower_bound; y_row <= m_flood_height.m_upper_bound; y_row ++) {
        for (int x_col = m_flood_width.m_lower_bound; x_col <= m_flood_width.m_upper_bound; x_col ++) {
            
            bool inRange = false;
            int inRangeCnt = 0;

            // depth_pixel palm area
            cv::Vec3b dp_palmrect = (*params.depth_mat).at<cv::Vec3b>(y_row, x_col); 
            
            int _col_mark = 255;
            for (int j = -1;j < 2; j ++) {

                if (cursor_y < j) continue;
                if (cursor_y + j >= params.m_frame_height) continue;
                
                
                for (int i = -1, _local_cnt = 0;i < 2; i ++) {
                    if (cursor_x < i) continue;
                    if (cursor_x + i >= params.m_frame_width) continue;
                    // depth pixel index cursor area
                    cv::Vec3b dp_cursor = (*params.depth_mat).at<cv::Vec3b>(cursor_y+j, cursor_x+i); 
                    
                
                    cv::circle(
                        (*params.depth_mat),
                        cv::Point(cursor_x+i, cursor_y+j),
                        10,
                        cv::Scalar(0, 140, 255),
                        cv::FILLED,
                        cv::LINE_8
                    );

                    for (int ch = 0; ch < 3; ch ++) {
                        int diff = std::abs(dp_cursor[ch] - dp_palmrect[ch]);
                        if (diff < m_floodfill_pixel_variance) {
                            _local_cnt ++;

                        }
                    }

                    if (_local_cnt > 1) inRangeCnt ++;
                }

            }

            if (inRangeCnt < 5) {
                _col_mark = 50;
                for (int j = -1;j < 2; j ++) {
                    if (cursor_midfinger_y < j) continue;
                    if (cursor_midfinger_y + j >= params.m_frame_height) continue;
                    for (int i = -1, _local_cnt = 0; i < 2; i ++) {
                        if (cursor_midfinger_x < i) continue;
                        if (cursor_midfinger_x + i >= params.m_frame_width) continue;
                        // depth pixel index cursor area
                        cv::Vec3b dp_cursor = (*params.depth_mat).at<cv::Vec3b>(cursor_midfinger_y+j, cursor_midfinger_x+i); 


                        for (int ch = 0; ch < 3; ch ++) {
                            int diff = std::abs(dp_cursor[ch] - dp_palmrect[ch]);
                            if (diff < m_floodfill_pixel_variance) {
                                _local_cnt ++;
                            }
                        }

                        if (_local_cnt > 1) inRangeCnt ++;
                    }
                }
            }

            
            if (inRangeCnt > 8) {
                cnt ++;
                dp_palmrect[0] = dp_palmrect[1] = 0;
                dp_palmrect[2] = _col_mark;
                (*params.depth_mat).at<cv::Vec3b>(y_row, x_col) = dp_palmrect;
            }
        }
    }
}


void TriggerTapDepth::get_channel_medians(cv::Mat * mat, int col_x, int row_y) {
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
