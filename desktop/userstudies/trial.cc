#include "trial.h"

namespace userstudies {
    Trial::Trial(int _divisions) {
        m_divisions = _divisions;
        // m_clicked = false;
        m_topleft = cv::Point(0, 0);
        m_bottomright = cv::Point(0, 0);
        m_start_btn = cv::Rect(
            m_topleft,
            m_bottomright
        );

        m_target_color = cv::Scalar(210,250,250);

        m_state = TrialState::OPEN;

        m_btn_color = cv::Scalar(255,255,0);

        m_start_btn_label = "START";
    }

    
    Trial::~Trial() {}

    
    void Trial::update_start_button_loc(Grid & grid) {
        switch (m_start_btn_loc) {
            case Location::LEFT:
                m_topleft = cv::Point(
                    std::max(0, grid.m_x_cols[0] - grid.m_dx_col - 20),
                    grid.m_y_rows[0]+grid.m_height - grid.m_dy_row
                );
                
                m_bottomright = cv::Point(
                    grid.m_x_cols[0],
                    grid.m_y_rows[0]+grid.m_height
                );

                m_start_btn = cv::Rect(
                    m_topleft,
                    m_bottomright
                );
                
                m_start_btn_label_loc_topleft = 
                    cv::Point(m_topleft.x + 5, (m_topleft.y+m_bottomright.y+10)/2);
                    

                break;
            case Location::CENTER:
                m_start_btn = grid.get_cell(
                    (grid.m_divisions+1)/2,
                    (grid.m_divisions+1)/2
                );

                break;
            case Location::LEFTCENTER:
                m_topleft = cv::Point(
                    std::max(0, grid.m_x_cols[0] - grid.m_dx_col - 20),
                    grid.m_y_rows[0]+grid.m_height/2 - grid.m_dy_row/2
                );
                
                m_bottomright = cv::Point(
                    grid.m_x_cols[0],
                    grid.m_y_rows[0]+grid.m_height/2 + grid.m_dy_row/2
                );

                m_start_btn = cv::Rect(
                    m_topleft,
                    m_bottomright
                );

                m_start_btn_label_loc_topleft = 
                    cv::Point(m_topleft.x + 5, (m_topleft.y+m_bottomright.y+10)/2);
                
        }
    }


    void Trial::draw_start_button(cv::Mat & output_frame) {

        if (m_state == TrialState::STARTED) {
            m_btn_color = cv::Scalar(220,248,255);
        }

        if (m_state == TrialState::OPEN || 
            m_state == TrialState::PAUSED || 
            m_trial_show_button_during_trial) {
            
            cv::rectangle(
                output_frame,
                m_start_btn,
                m_btn_color,
                -1,
                cv::LINE_8,
                0
            );

            cv::putText(
                output_frame,
                m_start_btn_label,
                m_start_btn_label_loc_topleft,
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 0),
                2
            );
        } 
    }


    void Trial::process_is_button_clicked(int cursor_x_col, int cursor_y_row) {
        if (m_start_btn.contains(
            cv::Point(cursor_x_col, cursor_y_row)
        )) {
            m_state = TrialState::STARTED;
            if (m_trial_pause_before_each_target) {
                m_start_btn_label = "NEXT";
            }
            
            _set_cur_target_start_time();


        } 
    }


    void Trial::generate_random_target_sequence(int n) {
        m_target_sequence.resize(n);
        m_time_taken.resize(n);
        m_trial_start_times.resize(n);
        m_trial_end_times.resize(n);

        for (int i = 0; i < n; i ++) {
            int j = rand() % (m_sample_space_size-i) + i;
            std::cout << "j:" << j << "\n";
            m_target_sequence[i] = m_sample_space[j];
        }

        m_target_sequence_size = n;
        m_target_id = 0;
    }


    void Trial::generate_sample_space() {

        if (m_start_btn_loc == Location::CENTER) {
            m_sample_space_size = (m_divisions*m_divisions) - 1;
        } else {
            m_sample_space_size = m_divisions*m_divisions;
        }

        // std::cerr << "m_sample_space_size:" << m_sample_space_size << "\n";
        m_sample_space.resize(m_sample_space_size);


        for (int i = 0; i < m_divisions; i ++) {
            // std::cerr << "i:" << i << "\n";
            for (int j = 0; j < m_divisions; j ++) {
                // std::cerr << "\tj:" << j << "\n";
                if (i == (m_divisions/2) && j == (m_divisions/2) && m_start_btn_loc == Location::CENTER) {
                    continue;
                }

                m_sample_space[(i*m_divisions) + j] = {i, j};
            }
        }

        // for (int i = 0 ;  i < m_sample_space_size; i ++) {
        //     std::cerr << "(" << m_sample_space[i].first << ", " << m_sample_space[i].second << ")\n"; 
        // }
    }


    std::pair<int, int> Trial::current_target() {
        if (done()) return {-1, -1};
        return m_target_sequence[m_target_id];
    } 


    bool Trial::done() {
        return (m_target_id == m_target_sequence_size);
    }
    

    void Trial::draw_target(cv::Mat & output_frame, Grid & grid) {
        
        if (m_state == TrialState::OPEN || m_state == TrialState::PAUSED) {
            return;
        }
        
        std::pair<int, int> cur = current_target();
        // assume current target is not {-1, -1}
        cv::Rect target = grid.get_cell(cur.first+1, cur.second+1);

        cv::rectangle(
            output_frame,
            target,
            m_btn_color,
            -1,
            cv::LINE_8,
            0
        );

    }


    bool Trial::matched(int row_i, int col_j) {
        // {row_i, col_j} initializer list does not work for lvalue comparison
        // row_[i,j] can be -1, meaning none of the cells of the grid is selected
        return std::make_pair(row_i, col_j) == current_target();
    }


    void Trial::process_correct_selection() {
        
        m_trial_end_times[m_target_id] = std::chrono::steady_clock::now();;
        
        m_time_taken[m_target_id] = 
                m_trial_end_times[m_target_id] - m_trial_start_times[m_target_id];
        
        m_target_id ++;

        if (m_trial_pause_before_each_target) {
            m_state = TrialState::PAUSED;
        } else {
            _set_cur_target_start_time();
        }
    }

    
    bool Trial::started() {
        return m_state == TrialState::STARTED;
    }


    void Trial::_set_cur_target_start_time() {
        if (done()) return;
        m_trial_start_times[m_target_id] = std::chrono::steady_clock::now();;
    }
};

