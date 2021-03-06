#ifndef DWELL_H
#define DWELL_H

#include "trigger.h"


class TriggerDwell: public Trigger {
    std::chrono::milliseconds m_timestamp[11][11], m_curtime, ptime, m_button_time;
    int selected_i, selected_j, selected_i_prv, selected_j_prv; 
    bool m_trial_button_focused;
    

    double m_progress_percentage;

    public:

    TriggerDwell();

    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);

    void _init_grid_state(const std::chrono::milliseconds & _current_timestamp);
    void reset_selection();
    void reset();
};


#endif