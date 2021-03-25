
#include "dwell.h"

// internal linkage, not visible outside
// for external linkage, use extern
constexpr int index_top = 8;
constexpr int DWELLWAIT_MS = 600;


TriggerDwell::TriggerDwell() {
    m_curtime = cur_time();
    _init_grid_state(m_curtime);
    reset_selection();

    cur_state = TRIGGER::OPEN;
    m_trial_button_focused = false;

    m_button_time = m_curtime;
    m_progress_percentage = 0;
}


void TriggerDwell::reset_selection() {
    selected_i = -1;
    selected_j = -1;
}


void TriggerDwell::_init_grid_state(const std::chrono::milliseconds & _current_timestamp) {
    for (int i = 0; i < 11; i ++) {
        for (int j = 0; j < 11; j ++) {
            m_timestamp[i][j] = m_curtime;
        }
    }
}


void TriggerDwell::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {

    m_curtime = cur_time();

    selected_i_prv = selected_i;
    selected_j_prv = selected_j;

    params.get_selected_cell(selected_i, selected_j);

    // std::cerr << "dwel update selected i:" << selected_i << "\tj:" << selected_j << "\n";
    if (cur_state == TRIGGER::OPEN) std::cerr << "state open\n";
    else if (cur_state == TRIGGER::ONHOLD) std::cerr << "state onhold\n";
    else if (cur_state == TRIGGER::PRESSED) std::cerr << "state pressed\n";
    else if (cur_state == TRIGGER::RELEASED) std::cerr << "state released\n";

    if (selected_i != -1) {
        if (selected_i_prv != -1) {
            if (selected_i_prv == selected_i && selected_j_prv == selected_j) {

                if (cur_state == TRIGGER::ONHOLD) {
                    std::cerr << "dwell onholded\n";
                } else {
                    ptime = m_timestamp[selected_i_prv][selected_j_prv];
                    // trigger if time diff larger than threshold
                    auto d = std::chrono::duration_cast<std::chrono::milliseconds>(m_curtime - ptime).count();

                    if (d > DWELLWAIT_MS) {
                        cur_state   = TRIGGER::RELEASED;
                        d           = DWELLWAIT_MS; // shortcut for setting progress bar to full
                    } else if (2*d > DWELLWAIT_MS) {
                        cur_state   = TRIGGER::PRESSED;
                    }

                    m_progress_percentage = (double)d/DWELLWAIT_MS;
                    // set progress for drawing progress bar in anchor
                    params.set_progress_bar(m_progress_percentage);
                }
            } else {
                std::cerr << "entered state change\n";
                cur_state = TRIGGER::OPEN;
                params.set_progress_bar(0);
                m_timestamp[selected_i][selected_j]         = m_curtime;
                m_timestamp[selected_i_prv][selected_j_prv] = m_curtime;
            }
        } else {
            cur_state = TRIGGER::OPEN;
            params.set_progress_bar(0);
            m_timestamp[selected_i][selected_j] = m_curtime;
        }
    } else {
        cur_state = TRIGGER::OPEN;
        params.set_progress_bar(0);
        if (selected_i_prv != -1) {
            m_timestamp[selected_i_prv][selected_j_prv] = m_curtime;
        } 
    }

    
    if (params.m_is_cursor_over_trial_button) {
        std::cerr << "dwell entered m_is_cursor_over_trial_button\n";
        cur_state = TRIGGER::OPEN;

        if (!m_trial_button_focused) {
            m_trial_button_focused  = true;
            m_button_time           = m_curtime;
        }

        auto d = std::chrono::duration_cast<std::chrono::milliseconds>(m_curtime - m_button_time).count();
        if (d > DWELLWAIT_MS) {
            cur_state   = TRIGGER::RELEASED;
            d           = DWELLWAIT_MS; // shortcut for setting progress bar to full
        } else if (2*d > DWELLWAIT_MS) {
            cur_state   = TRIGGER::PRESSED;
        }

        params.set_progress_bar((double)d/DWELLWAIT_MS);

    } else {
        m_trial_button_focused  = false;
        m_button_time           = m_curtime;
    }


}


void TriggerDwell::reset() {

    std::cerr << "reset called\n";
    m_cnt = 0;
    m_curtime = cur_time();
    cur_state = TRIGGER::ONHOLD;
    _init_grid_state(m_curtime);
}