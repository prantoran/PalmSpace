#ifndef TRIAL_H
#define TRIAL_H

#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "desktop/config/config.h"

// #include <chrono>

namespace userstudies {
    enum class Location{LEFT, CENTER, LEFTCENTER, RIGHTCENTER};

    enum class TrialState{OPEN, STARTED, PAUSED};
    
    enum class ButtonState {OPEN, CLICKED, READY};
        
    class Trial {
        public:
        Location m_start_btn_loc;
        cv::Rect m_start_btn;
        cv::Point m_topleft, m_bottomright;
        // bool m_clicked;
        cv::Scalar m_btn_color;

        int m_divisions;
        std::vector<std::pair<int, int>> m_target_sequence;

        std::vector<std::chrono::duration<double>> m_time_taken; 
        std::vector<std::chrono::time_point<std::chrono::steady_clock>> 
            m_trial_start_times, m_trial_end_times;

        int m_target_id;
        int m_target_sequence_size;

        std::vector<std::pair<int, int>> m_sample_space;
        int m_sample_space_size;

        cv::Scalar m_target_color;

        bool m_trial_pause_before_each_target;
        bool m_trial_show_button_during_trial;

        TrialState m_state;

        std::string m_start_btn_label;

        int m_view_width, m_view_height;

        std::chrono::time_point<std::chrono::steady_clock> m_last_trial_end_time;

        ButtonState m_button_state;

        Trial(int _divisions, int _view_width, int _view_height);
        ~Trial();
        void update_start_button_input_loc(
            const cv::Point & _topleft, 
            const cv::Point & _bottomright);
        void update_start_button_input_loc(Grid & grid);
        void draw_start_button(
            cv::Mat & output_frame,
            cv::Point _topleft,
            cv::Point _bottomright);
        void draw_start_button(cv::Mat & output_frame);
        void process_is_button_clicked(int cursor_x_col, int cursor_y_row);
        void generate_sample_space();
        void generate_random_target_sequence(int n);
        std::pair<int, int> current_target();
        bool done();
        void draw_target(cv::Mat & output_frame, const Grid & grid);
        bool matched(int row_i, int col_j);
        void process_correct_selection();
        bool started();
        void _set_cur_target_start_time();
    };
};

#endif