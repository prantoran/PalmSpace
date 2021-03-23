
#include "dwell.h"

// internal linkage, not visible outside
// for external linkage, use extern
constexpr int index_top = 8;
constexpr int DWELLWAIT_MS = 600;


TriggerDwell::TriggerDwell() {
    ctime = cur_time();
    _init_grid_state();

    m_trial_button_focused = false;

    m_button_time = ctime;
}


void TriggerDwell::_init_grid_state() {

    selected_i = -1;
    selected_j = -1;

    selected_i_prv = -1;
    selected_j_prv = -1;


    for (int i = 0; i < 11; i ++) {
        for (int j = 0; j < 11; j ++) {
            m_timestamp[i][j] = ctime;
        }
    }

    cur_state = TRIGGER::OPEN;
}


void TriggerDwell::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {

    ctime = cur_time();
    
    params.get_selected_cell(selected_i, selected_j);

    params.set_progress_bar(-1); // reset progress for drawing progress bar in anchor

    cur_state = TRIGGER::OPEN;


    std::cerr << "dwel update selected i:" << selected_i << "\tj:" << selected_j << "\n";

    if (selected_i != -1) {
        if (selected_i_prv != -1) {
            if (selected_i_prv == selected_i && selected_j_prv == selected_j) {
                ptime = m_timestamp[selected_i_prv][selected_j_prv];
                // trigger if time diff larger than threshold
                auto d = std::chrono::duration_cast<std::chrono::milliseconds>(ctime - ptime).count();

                if (d > DWELLWAIT_MS) {
                    cur_state = TRIGGER::RELEASED;
                    d = DWELLWAIT_MS; // shortcut for setting progress bar to full
                }
                // set progress for drawing progress bar in anchor
                params.set_progress_bar((double)d/DWELLWAIT_MS);

            } else {
                m_timestamp[selected_i][selected_j] = ctime;
                m_timestamp[selected_i_prv][selected_j_prv] = ctime;
            }
        } else {
            m_timestamp[selected_i][selected_j] = ctime;
        }
    } else {
        if (selected_i_prv != -1) {
            m_timestamp[selected_i_prv][selected_j_prv] = ctime;
        } 
    }

    selected_i_prv = selected_i;
    selected_j_prv = selected_j;
    
    if (params.m_is_cursor_over_trial_button) {
        _init_grid_state();
        cur_state = TRIGGER::OPEN;
        
        if (!m_trial_button_focused) {
            m_trial_button_focused = true;
            m_button_time = ctime;
        }

        auto d = std::chrono::duration_cast<std::chrono::milliseconds>(ctime - m_button_time).count();
        if (d > DWELLWAIT_MS) {
            cur_state = TRIGGER::RELEASED;
            d = DWELLWAIT_MS; // shortcut for setting progress bar to full
        } 

        params.set_progress_bar((double)d/DWELLWAIT_MS);

    } else {
        m_trial_button_focused = false;
        m_button_time = ctime;
    }


}
