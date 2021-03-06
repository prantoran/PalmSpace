#include "trial.h"
#include "../ui/ui.h"

namespace userstudies {
    Trial::Trial(int _divisions, int _view_width, int _view_height) {
        m_divisions = _divisions;
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


    void Trial::update_start_button_input_loc(cv::Mat & output_frame) {
        m_topleft     = cv::Point(-50 + output_frame.cols/2, -40 + output_frame.rows/2);
        m_bottomright = cv::Point(+50 + output_frame.cols/2, +40 + output_frame.rows/2); 

        m_start_btn = cv::Rect(
            m_topleft,
            m_bottomright
        );
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
                cv::Point(-30 + (_topleft.x + _bottomright.x)/2, (_topleft.y+_bottomright.y+10)/2),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 0),
                2
            );

            if (m_button_state == ButtonState::OPEN) {
                m_button_state = ButtonState::READY;
                m_last_trial_end_time = std::chrono::steady_clock::now();
            }
        } 
    }


    void Trial::draw_start_button(cv::Mat & output_frame) {
        draw_start_button(output_frame, m_topleft, m_bottomright); 
    }


    bool Trial::is_cursor_over_trial_button(int cursor_x_col, int cursor_y_row) {

        if (m_state == TrialState::STARTED) {
            return false;
        }

        auto diff_ms =  std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - m_last_trial_end_time).count();
        
        if (m_state == TrialState::OPEN and diff_ms < 1000) {
            return false;
        }

        if (diff_ms < 800) {
            return false;
        }

        return m_start_btn.contains(
            cv::Point(
                cursor_x_col, 
                cursor_y_row
            )
        );
    }


    void Trial::process_button_clicked() {
        m_state = TrialState::STARTED;
        if (m_trial_pause_before_each_target) {
            m_start_btn_label = "NEXT";
        }
        
        _set_cur_target_start_time();
    }


    void Trial::init_datastores(int n) {
        m_target_sequence.resize(n);
        m_time_taken_ms.resize(n);
        m_trial_start_times.resize(
            n, std::chrono::time_point<std::chrono::steady_clock>());
        m_trial_end_times.resize(
            n, std::chrono::time_point<std::chrono::steady_clock>());
        m_dist_traveled_px.resize(n, 0);
        m_targets_last_visited_time_ms.resize(n);

        m_last_cursor_pos_for_targets.resize(n, std::make_pair(-1, -1));

        m_attempts.resize(n, 0);
        m_visited_cells.resize(n, 0);
        
        m_target_sequence[0] = m_sample_space[0];
        

        m_target_sequence_size = n;
        m_target_id = 0;
        m_sample_space_id = 0;
    
        for (int i = 0;i < 2; i ++) {
            m_dist_travelled_palms[i].resize(n, 0);
            m_palm_base_last_pos[i].resize(n, std::make_pair(-1, -1));
        }
    }


    void Trial::generate_sample_space() {

        m_sample_space_size = m_divisions*m_divisions;

        m_sample_space.resize(m_sample_space_size);

        for (int i = 0; i < m_divisions; i ++) {
            for (int j = 0; j < m_divisions; j ++) {
                m_sample_space[(i*m_divisions) + j] = {i, j};
            }
        }

        for (int i = 0; i < m_sample_space_size; i ++) {
            int j = rand() % (m_sample_space_size-i) + i;
            std::swap(m_sample_space[i], m_sample_space[j]);
        }
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


    void Trial::update_cur_target_time() {
        
        m_trial_end_times[m_target_id] = std::chrono::steady_clock::now();

        m_time_taken_ms[m_target_id] = 
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    m_trial_end_times[m_target_id] - 
                    m_trial_start_times[m_target_id]
                ).count();

        m_last_trial_end_time = std::chrono::steady_clock::now(); 
    }


    void Trial::move_to_next_target(int prev_marked_i, int prev_marked_j) {
        // prev marked i, j indices start from 1
        // trial i, j indices start from 0
        m_target_id ++;

        while (true) {
            m_sample_space_id = (m_sample_space_id + 1) % m_sample_space_size;
            
            const std::pair<int, int> & p = m_sample_space[m_sample_space_id];

            if (p.first != (prev_marked_i) || p.second != (prev_marked_j)) {
                m_target_sequence[m_target_id] = p;
                break;
            }
        }
        
        if (m_trial_pause_before_each_target) {
            m_state = TrialState::PAUSED;
        } 
            
        _set_cur_target_start_time();
    }

    
    bool Trial::started() {
        return m_state == TrialState::STARTED;
    }


    void Trial::_set_cur_target_start_time() {
        if (done()) return;
        m_trial_start_times[m_target_id] = std::chrono::steady_clock::now();;
    }

    void Trial::update_cur_target_distance_traveled(int cursor_x_col, int cursor_y_row) {

        if (cursor_x_col > 0 && cursor_y_row > 0) {

            const std::pair<int, int> & p = m_last_cursor_pos_for_targets[m_target_id];

            if (p.first > 0 && p.second > 0) {
                m_dist_traveled_px[m_target_id] += sqrt(
                                                        (cursor_x_col-p.first)*(cursor_x_col-p.first) + 
                                                        (cursor_y_row-p.second)*(cursor_y_row-p.second)
                                                    );
            } 

        }

        m_last_cursor_pos_for_targets[m_target_id] = {cursor_x_col, cursor_y_row};
    }


    void Trial::increment_attempts() {
        m_attempts[m_target_id] ++;
    }

    
    void Trial::update_left_palm_distance(int palmbase_x, int palmbase_y) {
        _upd_palm_dist(0, palmbase_x, palmbase_y);
    }


    void Trial::update_right_palm_distance(int palmbase_x, int palmbase_y) {
        _upd_palm_dist(1, palmbase_x, palmbase_y);
    }


    void Trial::_upd_palm_dist(int palm_id, int _x, int _y) {
        
        const std::pair<int, int> & p = m_palm_base_last_pos[palm_id][m_target_id];
        
        if (p.first >= 0 && p.second >= 0 && _x >= 0 && _y >= 0) {
            m_dist_travelled_palms[palm_id][m_target_id] += sqrt(
                                                    (_x - p.first)  * (_x - p.first) +
                                                    (_y - p.second) * (_y - p.second)
                                                );   
        }

        m_palm_base_last_pos[palm_id][m_target_id] = {_x, _y};
    }


    void Trial::draw_completed_targets_text(cv::Mat & output_frame) {
        cv::putText(
            output_frame,
            std::to_string(m_target_id) + "/" + std::to_string(m_target_sequence_size),
            cv::Point(output_frame.cols-110, output_frame.rows-20),
            cv::FONT_HERSHEY_DUPLEX,
            1.0,
            cv::Scalar(240, 240, 200),
            2);
    }

    void Trial::update_target_last_visited_time(const std::chrono::time_point<std::chrono::steady_clock> & _last_visited_time) {
        m_targets_last_visited_time_ms[m_target_id] =  
            std::chrono::duration_cast<std::chrono::milliseconds>(
                _last_visited_time - m_trial_start_times[m_target_id]
            ).count();

        if (m_targets_last_visited_time_ms[m_target_id] < 0) {
            m_targets_last_visited_time_ms[m_target_id] = 0;
        }

    }

    void Trial::draw_debug(cv::Mat & output_frame) {
        cv::putText(
            output_frame,
            "target_last_visited_time_ms:" + std::to_string((int)m_targets_last_visited_time_ms[m_target_id]) +"ms",
            cv::Point(20, output_frame.rows-20),
            cv::FONT_HERSHEY_DUPLEX,
            0.8,
            cv::Scalar(240, 240, 200),
            1
        );

        double _d = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - m_trial_start_times[m_target_id]).count();
        cv::putText(
            output_frame,
            "elapsed_time:" + std::to_string((int)(_d)) + "ms",
            cv::Point(20, output_frame.rows-50),
            cv::FONT_HERSHEY_DUPLEX,
            0.8,
            cv::Scalar(240, 240, 200),
            1
        );

        cv::putText(
            output_frame,
            "time_taken_ms:" + std::to_string((int)m_time_taken_ms[m_target_id]) + "ms",
            cv::Point(20, output_frame.rows-80),
            cv::FONT_HERSHEY_DUPLEX,
            0.8,
            cv::Scalar(240, 240, 200),
            1
        );

        cv::putText(
            output_frame,
            "distance_traveled_px:" + std::to_string((int)m_dist_traveled_px[m_target_id]) + "px",
            cv::Point(20, output_frame.rows-110),
            cv::FONT_HERSHEY_DUPLEX,
            0.8,
            cv::Scalar(240, 240, 200),
            1
        );

        cv::putText(
            output_frame,
            "attempts:" + std::to_string((int)m_attempts[m_target_id]),
            cv::Point(20, output_frame.rows-140),
            cv::FONT_HERSHEY_DUPLEX,
            0.8,
            cv::Scalar(240, 240, 200),
            1
        );

        cv::putText(
            output_frame,
            "m_visited_cells:" + std::to_string((int)m_visited_cells[m_target_id]),
            cv::Point(20, output_frame.rows-170),
            cv::FONT_HERSHEY_DUPLEX,
            0.8,
            cv::Scalar(240, 240, 200),
            1
        );
    }

};

