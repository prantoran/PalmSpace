#ifndef TAP_H
#define TAP_H


#include "trigger.h"


class TriggerTap: public Trigger {
    public:

    double m_base_rel_depth, m_diff;

    TriggerTap();
    
    void update(
        const cv::Mat & input_image,
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);
    void reset();
};

#endif