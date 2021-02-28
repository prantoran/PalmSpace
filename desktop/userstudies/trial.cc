#include "trial.h"
#include "../ui/ui.h"

namespace userstudies {
    Trial::Trial(int _divisions, int _view_width, int _view_height) {
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

        m_view_width = _view_width;
        m_view_height = _view_height;

        m_button_state = ButtonState::OPEN;
    }

    
    Trial::~Trial() {}


    void Trial::update_start_button_input_loc(const cv::Point & _topleft, const cv::Point & _bottomright) {
        m_topleft = _topleft;
        m_bottomright = _bottomright;
        
        m_start_btn = cv::Rect(
            m_topleft,
            m_bottomright
        );

    }


    void Trial::update_start_button_input_loc(Grid & grid) {

        std::cerr << "trial updloc grid width:" << grid.m_width << "\theight:" << grid.m_height << "\twidth_min:" << grid.m_width_min << "\theight_min:" << grid.m_height_min << "\n";

        switch (m_start_btn_loc) {
            case Location::LEFT:
        
                update_start_button_input_loc(
                    cv::Point(
                        std::max(0.0, grid.m_x_cols[0] - grid.m_dx_col - 20),
                        grid.m_y_rows[0]+grid.m_height - grid.m_dy_row),
                    cv::Point(
                        grid.m_x_cols[0],
                        grid.m_y_rows[0]+grid.m_height)
                );
                
                break;

            case Location::CENTER:
                m_start_btn = grid.get_cell(
                    (grid.m_divisions+1)/2,
                    (grid.m_divisions+1)/2
                );

                break;
            case Location::LEFTCENTER:
                
                update_start_button_input_loc(
                    cv::Point(
                        std::max(0.0, grid.m_x_cols[0] - grid.m_dx_col - 20),
                        grid.m_y_rows[0]+grid.m_height/2 - grid.m_dy_row),
                    cv::Point(
                        grid.m_x_cols[0],
                        grid.m_y_rows[0]+grid.m_height/2 + grid.m_dy_row)
                );

                break;

            case Location::RIGHTCENTER:
                std::cerr << "RIGHTCENTER\n";
                
                update_start_button_input_loc(
                    cv::Point(
                        std::min(m_view_width, (int)(grid.m_x_cols[0] + grid.m_width_min)),
                        grid.m_y_rows[0]+grid.m_height/2 - grid.m_dy_row),
                    cv::Point(
                        std::min(m_view_width, (int)(grid.m_x_cols[0] + grid.m_width_min + grid.m_dx_col + 20)),
                        grid.m_y_rows[0]+grid.m_height/2 + grid.m_dy_row)
                );
                
                break;
            
            default:
                std::cout << "ERROR: userstudies/trial.cc update_start_button_input_loc() undefined\n";
                break;
        }
    }


    void Trial::draw_start_button(
        cv::Mat & output_frame,
        cv::Point _topleft,
        cv::Point _bottomright) {
        
        
        
        if (m_state == TrialState::STARTED) {
            m_btn_color = cv::Scalar(220,248,255);
        }

        if (m_state == TrialState::OPEN || 
            m_state == TrialState::PAUSED || 
            m_trial_show_button_during_trial) {
            

            ui::rounded_rectangle(
                output_frame,
                _topleft,
                _bottomright,
                m_btn_color,
                5,
                cv::LINE_8,
                5
            );

            cv::putText(
                output_frame,
                m_start_btn_label,
                cv::Point(_topleft.x + 5, (_topleft.y+_bottomright.y+10)/2),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 0),
                2
            );

            if (m_button_state == ButtonState::OPEN) {
                m_button_state = ButtonState::READY;
                // std::cout << "button state from open to ready\n";
                m_last_trial_end_time = std::chrono::steady_clock::now();
            }
        } 
    }


    void Trial::draw_start_button(cv::Mat & output_frame) {

        if (m_state == TrialState::STARTED) {
            m_btn_color = cv::Scalar(220,248,255);
        }

        if (m_state == TrialState::OPEN || 
            m_state == TrialState::PAUSED || 
            m_trial_show_button_during_trial) {

            ui::rounded_rectangle(
                output_frame,
                m_topleft,
                m_bottomright,
                m_btn_color,
                5,
                cv::LINE_8,
                5
            );

            cv::putText(
                output_frame,
                m_start_btn_label,
                cv::Point(m_topleft.x + 5, (m_topleft.y+m_bottomright.y+10)/2),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 0),
                2
            );

            if (m_button_state == ButtonState::OPEN) {
                m_button_state = ButtonState::READY;
                // std::cout << "button state from open to ready\n";
                m_last_trial_end_time = std::chrono::steady_clock::now();
            }
        } 
    }





    void Trial::process_is_button_clicked(int cursor_x_col, int cursor_y_row) {
        
        auto diff =  (std::chrono::steady_clock::now() - m_last_trial_end_time).count();
        // std::cout << "diff:" << diff <<  "\n";
        if (m_state == TrialState::OPEN and diff < 2000000000) {
            return ;
        }

        if (diff < 1000000000) {
                return;
        }
        
        
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
        m_trial_start_times.resize(
            n, std::chrono::time_point<std::chrono::steady_clock>());
        m_trial_end_times.resize(
            n, std::chrono::time_point<std::chrono::steady_clock>());

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
    

    void Trial::draw_target(cv::Mat & output_frame, const Grid & grid) {
        
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
        
        m_trial_end_times[m_target_id] = std::chrono::steady_clock::now();

        m_time_taken[m_target_id] = 
                m_trial_end_times[m_target_id] - m_trial_start_times[m_target_id];
        
        m_target_id ++;

        if (m_trial_pause_before_each_target) {
            m_state = TrialState::PAUSED;
        } else {
            _set_cur_target_start_time();
        }

        m_last_trial_end_time = std::chrono::steady_clock::now(); 
    }

    
    bool Trial::started() {
        return m_state == TrialState::STARTED;
    }


    void Trial::_set_cur_target_start_time() {
        if (done()) return;
        m_trial_start_times[m_target_id] = std::chrono::steady_clock::now();;
    }
};

