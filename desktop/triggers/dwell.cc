
#include "triggers.h"

// internal linkage, not visible outside
// for external linkage, use extern
constexpr int index_top = 8;
constexpr int DWELLWAIT_MS = 1300;

TriggerDwell::TriggerDwell() {

    selected_i = -1;
    selected_j = -1;

    selected_i_prv = -1;
    selected_j_prv = -1;

    ctime = cur_time();

    for (int i = 0; i < 11; i ++) {
        for (int j = 0; j < 11; j ++) {
            _timestamp[i][j] = ctime;
        }
    }

    cur_state = TRIGGER::OPEN;
}

void TriggerDwell::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    ExtraParameters & params) {

    ctime = cur_time();
    
    params.get_selected_cell(selected_i, selected_j);

    params.extra_params[9] = -1; // reset progress for drawing progress bar in anchor

    if (selected_i != -1) {
        if (selected_i_prv != -1) {
            if (selected_i_prv == selected_i && selected_j_prv == selected_j) {
                ptime = _timestamp[selected_i_prv][selected_j_prv];
                // trigger if time diff larger than threshold
                auto d = std::chrono::duration_cast<std::chrono::milliseconds>(ctime - ptime).count();

                if (d > DWELLWAIT_MS) {
                    cur_state = TRIGGER::RELEASED;
                    d = DWELLWAIT_MS; // shortcut for setting progress bar to full
                }

                // set progress for drawing progress bar in anchor
                params.extra_params[9] = (double)d/DWELLWAIT_MS;

            } else {

                _timestamp[selected_i][selected_j] = ctime;
                _timestamp[selected_i_prv][selected_j_prv] = ctime;

                cur_state = TRIGGER::OPEN;
            }
        } else {
            _timestamp[selected_i][selected_j] = ctime;
            cur_state = TRIGGER::OPEN;
        }
    } else {
        if (selected_i_prv != -1) {
            _timestamp[selected_i_prv][selected_j_prv] = ctime;
        } else {
            // do nothing
        }

        cur_state = TRIGGER::OPEN;
    }

    selected_i_prv = selected_i;
    selected_j_prv = selected_j;
}

TRIGGER::state TriggerDwell::status() {
    return cur_state;
}