#ifndef STUDY_H
#define STUDY_H

#include <cmath> 
#include <vector>
#include <string>
#include <iostream>
#include "desktop/util/util.h"

namespace userstudies {
    class Study {
        public:
        std::string m_filename, m_events_dir;
        std::vector<std::string> m_headers, m_event_headers;
        std::string m_trial_counter_file_loc;

        void save_row(const std::string & _row);
        void init_file_with_headers();

        void increment_trial_counter();

        void save_event_row(const std::string & _eventfile, const std::string & _row);
        void init_file_with_event_headers(const std::string & _eventfile);
    };


    class Study1: public Study {
        public:
        int m_grid_cell_cnt_per_axis;
        std::string m_selection, m_technique;
        int m_no_of_grids;

        Study1(
            std::string _filename, 
            std::string _trial_counter_file_loc,
            std::string _events_dir,
            std::string _technique, 
            std::string _selection, 
            int _no_of_grids);
        
        void save(
            int _target_id, const std::pair<int, int> & _target_loc_indices, 
            double _time_taken_ms, double _dist_traveled_px,
            int _attempts, int _visited_cells,
            double _dist_travelled_left_hand, double _dist_travelled_right_hand);

        void update_event(
            int _target_id, const std::pair<int, int> & _target_loc_indices, 
            double _time_taken_ms, double _dist_traveled_px,
            int _attempts, int _visited_cells,
            double _dist_travelled_left_hand, double _dist_travelled_right_hand,
            const std::string & _lefthand_landmarks,
            const std::string & _righthand_landmarks);

    };
}


#endif