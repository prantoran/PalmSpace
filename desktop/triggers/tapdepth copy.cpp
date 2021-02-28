#include "triggers.h"

#include <queue>
#include <utility>
#include <unordered_map>

// #include "mediapipe/framework/port/opencv_highgui_inc.h" // GUI #include "opencv2/highgui/highgui.hpp"
// #include "mediapipe/framework/port/opencv_imgproc_inc.h"
// #include "mediapipe/framework/port/opencv_video_inc.h"

SupportPoints::SupportPoints() {}

void SupportPoints::init(int _size) {
    m_support_pts_size = _size;

    m_xcoords       = std::vector<int>(m_support_pts_size);
    m_xcoords_prev  = std::vector<int>(m_support_pts_size);
    m_ycoords       = std::vector<int>(m_support_pts_size);
    m_ycoords_prev  = std::vector<int>(m_support_pts_size);

    for (int i = 0; i < m_support_pts_size; i ++) {
        m_xcoords[i] = -1;
        m_ycoords[i] = -1;
    }

}


void SupportPoints::backup_prev() {
    for (int k = 0; k < m_support_pts_size; k ++) {
        m_xcoords_prev[k] = m_xcoords[k];
        m_ycoords_prev[k] = m_ycoords[k];
    }
}


void SupportPoints::update_support(
    int indices[], 
    const std::vector<std::tuple<double, double, double>> & coords,
    const Parameters & params
) {
    // std::cerr << "update_support begin\n";
    for (int i = 0; i < m_support_pts_size; i ++) {
        // std::cerr << "indices[" << i << "]: " << indices[i] << "\n";
        m_xcoords[i] = (int)(params.m_frame_width  * std::get<0>(coords[indices[i]]));
        m_ycoords[i] = (int)(params.m_frame_height * std::get<1>(coords[indices[i]]));
    
        if (m_xcoords_prev[i] != -1) {
            m_xcoords[i] = (1-0.9)*m_xcoords[i] + (0.9)*m_xcoords_prev[i];
        }


        if (m_ycoords_prev[i] != -1) {
            m_ycoords[i] = (1-0.9)*m_ycoords[i] + (0.9)*m_ycoords_prev[i];
        }
    }
    // std::cerr << "update_support end\n";



}

// TODO overload to print using cout <<
void SupportPoints::print() {
    for (int i = 0; i < m_support_pts_size; i ++) {
        std::cout << "(" << m_xcoords[i] << ", " << m_ycoords[i] << ")\t";
    } std::cout << "\n";
}




// Only for pairs of std::hash-able types for simplicity.
// You can of course template this struct to allow other hash functions
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;  
    }
};


TriggerTapDepth::TriggerTapDepth() {
    m_depth_base = -1;
    m_depth_cursor = -1;

    m_base_pts.init(2);
    m_cursor_pts.init(2);

    for (int k = 0, i = 10; i < 15; i += 4, k ++) {
        m_base_indices[k] = i;
    }


    // for (int k = 0, i = 7; i <=8; i++, k ++) {
    //     m_cursor_indices[k] = i;
    // }
    for (int k = 0, i = 9; i <=13; i+=4, k ++) {
        m_cursor_indices[k] = i;
    }
    // m_cursor_indices[2] = 9;


    // channel medians start
    m_channel_cnt = 3;
    m_floodfill_pixel_variance = 40;
    positive_frames_cnt = 0;
    // end

    cur_state = TRIGGER::OPEN;

}

void TriggerTapDepth::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {

    m_base_pts.backup_prev();   
    m_base_pts.update_support(m_base_indices, points[0], params);
    // std::cerr << "trigdepth base_points:\n";
    // m_base_pts.print();

    m_cursor_pts.backup_prev();
    m_cursor_pts.update_support(m_cursor_indices, points[1], params);
    // std::cerr << "trigdepth cursor_points:\n";
    // m_base_pts.print();

    // process_depths();

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

    if (cnt > 2500) {
        positive_frames_cnt ++;
        if (positive_frames_cnt > 3) 
            positive_frames_cnt = 3;
    } else {
        positive_frames_cnt --;
        if (positive_frames_cnt < 0) 
            positive_frames_cnt = 0;
    }
    std::cerr << "cnt:" << cnt << " positive_frames_cnt:" << positive_frames_cnt << "\n";

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
        std::max(0, x_col_palmid - 50), 
        std::min(x_col_palmid + 50, params.m_frame_height-1));

    m_flood_height = Range(
        std::max(0, y_row_palmid-50), 
        std::min(y_row_palmid + 50, params.m_frame_height-1));
    
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

            for (int j = -1;j < 2; j ++) {

                if (cursor_y < j) continue;
                if (cursor_y + j >= params.m_frame_height) continue;
                
                
                for (int i = -1;i < 2; i ++) {
                    if (cursor_x < i) continue;
                    if (cursor_x + i >= params.m_frame_width) continue;
                    // depth pixel index cursor area
                    cv::Vec3b dp_cursor = (*params.depth_mat).at<cv::Vec3b>(cursor_y+j, cursor_x+i); 
                    
                
                    cv::circle(
                        (*params.depth_mat),
                        cv::Point(cursor_x+i, cursor_y+j),
                        10,
                        cv::Scalar(192, 192, 192),
                        cv::FILLED,
                        cv::LINE_8
                    );

                    for (int ch = 0; ch < 3; ch ++) {
                        int diff = std::abs(dp_cursor[ch] - dp_palmrect[ch]);
                        if (diff < m_floodfill_pixel_variance) {
                            inRangeCnt ++;
                        }
                    }
                }

            }

            
            for (int j = -1;j < 2; j ++) {
                if (cursor_midfinger_y < j) continue;
                if (cursor_midfinger_y + j >= params.m_frame_height) continue;
                for (int i = -1;i < 2; i ++) {
                    if (cursor_midfinger_x < i) continue;
                    if (cursor_midfinger_x + i >= params.m_frame_width) continue;
                    // depth pixel index cursor area
                    cv::Vec3b dp_cursor = (*params.depth_mat).at<cv::Vec3b>(cursor_midfinger_y+j, cursor_midfinger_x+i); 


                    for (int ch = 0; ch < 3; ch ++) {
                        int diff = std::abs(dp_cursor[ch] - dp_palmrect[ch]);
                        if (diff < m_floodfill_pixel_variance) {
                            inRangeCnt ++;
                        }
                    }
                }
            }
            
            if (inRangeCnt > 36) {
                cnt ++;
                dp_palmrect[0] = dp_palmrect[1] = 0;
                dp_palmrect[2] = 255;
                (*params.depth_mat).at<cv::Vec3b>(y_row, x_col) = dp_palmrect;
            }
        }
    }
}


void TriggerTapDepth::process_depths(Parameters & params) {

    m_depth_base_prev = m_depth_base;
    m_depth_cursor_prev = m_depth_cursor;

    params.get_depth_at(m_base_pts.m_xcoords, m_base_pts.m_ycoords, m_depth_base);
    params.get_depth_at(m_cursor_pts.m_xcoords, m_cursor_pts.m_ycoords, m_depth_cursor);

    if (m_depth_base_prev != -1) {
        m_depth_base   = (1-0.9)*m_depth_base   + (0.9)*m_depth_base_prev;
    }

    if (m_depth_cursor_prev != -1) {
        m_depth_cursor = (1-0.9)*m_depth_cursor + (0.9)*m_depth_cursor_prev;
    }

    std::cerr << "depth depth_base:" << m_depth_base << "m\tdepth_cursor:" << m_depth_cursor << "m\n";

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


void TriggerTapDepth::flood_fill(Parameters & params, int & cnt) {
    // use cursor x, y as the seed and top-mid position of the filling rectangular region
    // modifies the params.depth_mat
    int cursor_x, cursor_y;
    params.get_primary_cursor_cv_indices(cursor_x, cursor_y);

    m_flood_width = Range(
        std::max(0, cursor_x - 50), 
        std::min(cursor_x + 50, params.m_frame_height-1));

    m_flood_height = Range(
        cursor_y, 
        std::min(params.m_frame_height-1, cursor_y+100));

    params.m_flood_height = m_flood_height;
    params.m_flood_width = m_flood_width;

     
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};

    Range ch[3];
    cv::Vec3b cursor_pixel = (*params.depth_mat).at<cv::Vec3b>(cursor_y, cursor_x);
    for (int i = 0; i < 3; i ++) {
        // int mid_pixel = m_ch_medians[i];
        int mid_pixel = cursor_pixel[i];

        ch[i] = Range(mid_pixel-m_floodfill_pixel_variance,
                    mid_pixel+m_floodfill_pixel_variance);
    }

    cnt = 0;

    std::queue<std::pair<int, int>> q;
    std::unordered_map<std::pair<int, int>, bool, pair_hash> visited;
    
    for (int i = -5; i < 6; i ++) {
        if (cursor_x < i) continue;
        for (int j = -5; j < 6; j ++) {
            if (cursor_y < j) continue;
            q.push({cursor_x+i, cursor_y+j});
            visited[{cursor_x+i, cursor_y+j}] = true;
        }
    }

    while (!q.empty()) {
        std::pair<int, int> u = q.front();
        q.pop();

        int x_col = u.first;
        int y_row = u.second;

        // first = width, second = height
        cv::Vec3b u_pixel = (*params.depth_mat).at<cv::Vec3b>(y_row, x_col);

        int inRange = 0;
        for (int i = 0;i < 3; i ++) {
            if (ch[i].isInside(u_pixel[i])) 
                inRange ++;
        }

        if (inRange > 1) {
            cnt ++;
            u_pixel[0] = 255;
            u_pixel[1] = 255;
            u_pixel[2] = 0;

            // WARN modifying depth map
            (*params.depth_mat).at<cv::Vec3b>(y_row, x_col) = u_pixel;
        } else {
            continue;
        }

        for (int i = 0; i < 4; i ++) {
            int x_new = x_col + dx[i];
            int y_new = y_row + dy[i];

            if (!m_flood_width.isInside(x_new) || 
                !m_flood_height.isInside(y_new))
                continue;
            
            if (visited[{x_new, y_new}]) continue;

            visited[{x_new, y_new}] = true;

            q.push({x_new, y_new});
        }
    }

}