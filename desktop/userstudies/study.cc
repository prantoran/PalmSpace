
#include "study.h"

namespace userstudies {
    void Study::save_row(const std::string & _row) {
        // saves row to file
        if (!util::file_exists(m_filename)) {
            init_file_with_headers();
        }

        std::fstream fout;
        
        fout.open(m_filename, std::ios::app);

        fout << _row << "\n";

        fout.close();
    }


    void Study::save_event_row(const std::string & _eventfile, const std::string & _row) {
        
        std::cout << "save_event_row() _eventfile:" << _eventfile << "\n";
        
        if (!util::file_exists(_eventfile)) {
            init_file_with_event_headers(_eventfile);
        }

        std::fstream fout;
        fout.open(_eventfile, std::ios::app);
        fout << _row << "\n";
        fout.close();
    }


    void Study::init_file_with_headers() {

        std::cout << "userstudies/study.cc init_file_with_headers() called m_filename:" << m_filename << "\n";
        std::fstream fout;
        fout.open(m_filename, std::ios::out);
        
        for (int i = 0; i < m_headers.size(); i ++) {
            std::cout << "header[" << i << "]: " << m_headers[i] << "\n"; 
            if (i) fout << ",";
            fout << m_headers[i];
        }
        fout << "\n";

        fout.close();
    }


    void Study::init_file_with_event_headers(const std::string & _eventfile) {
        std::cout << "userstudies/study.cc init_file_with_event_headers() called _eventfile:" << _eventfile << "\n";
        std::fstream fout;
        fout.open(_eventfile, std::ios::out);
        
        for (int i = 0; i < m_event_headers.size(); i ++) {
            std::cout << "header[" << i << "]: " << m_event_headers[i] << "\n"; 
            if (i) fout << ",";
            fout << m_event_headers[i];
        }
        fout << "\n";

        fout.close();
    }


    void Study::increment_trial_counter() {
        std::fstream fin(m_trial_counter_file_loc, std::ios_base::in);
        int trial_id;
        fin >> trial_id;
        fin.close();
        trial_id ++;
        std::fstream fout(m_trial_counter_file_loc, std::ios_base::out);
        fout << trial_id;
        fout.close();
    }


    Study1::Study1(
        std::string _filename, 
        std::string _trial_counter_file_loc, 
        std::string _events_dir,
        std::string _technique, 
        std::string _selection, 
        int _no_of_grids) {
        
        m_filename                  = _filename;
        m_trial_counter_file_loc    = _trial_counter_file_loc;
        m_events_dir                = _events_dir;

        m_technique     = _technique;
        m_selection     = _selection;
        m_no_of_grids   = _no_of_grids;
        m_headers = {
            "trial_id",
            "selection", "technique", "no_of_grids", 
            "target_no", "target_location", 
            "time_taken_ms", "distance_traveled_px",
            "no_of_attempts", "no_of_visited_cells",
            "tot_dist_travelled_left_palm", "tot_dist_travelled_right_palm"
        };

        m_event_headers = {
            "trial_id",
            "selection", "technique", "no_of_grids", 
            "target_no", "target_location", 
            "time_taken_ms", "distance_traveled_px",
            "no_of_attempts", "no_of_visited_cells",
            "tot_dist_travelled_left_palm", "tot_dist_travelled_right_palm",
            "landmarks_left", "landmarks_right"
        };
    }


    void Study1::save(
        int _target_id, const std::pair<int, int> & _target_loc_indices, 
        double _time_taken_ms, double _dist_traveled_px,
        int _attempts, int _visited_cells,
        double _dist_travelled_left_hand, double _dist_travelled_right_hand) {
        

        std::fstream fin(m_trial_counter_file_loc, std::ios_base::in);

        std::string trial_id;
        fin >> trial_id;

        fin.close();

        std::string row = trial_id + "," + m_selection + "," + m_technique + "," + std::to_string(m_no_of_grids);

        row = row + "," + std::to_string(_target_id)                + ",\"("  + std::to_string(_target_loc_indices.first)  + "," + std::to_string(_target_loc_indices.second) + ")\"";

        row = row + "," + std::to_string(_time_taken_ms)            + ","     + std::to_string(_dist_traveled_px);

        row = row + "," + std::to_string(_attempts)                 + ","     + std::to_string(_visited_cells); 

        row = row + "," + std::to_string(_dist_travelled_left_hand) + ","     + std::to_string(_dist_travelled_right_hand);

        save_row(row);
    }


    void Study1::update_event(
        int _target_id, const std::pair<int, int> & _target_loc_indices, 
        double _time_taken_ms, double _dist_traveled_px,
        int _attempts, int _visited_cells,
        double _dist_travelled_left_hand, double _dist_travelled_right_hand,
        const std::string & _lefthand_landmarks,
        const std::string & _righthand_landmarks) {

        std::fstream fin(m_trial_counter_file_loc, std::ios_base::in);
        std::string trial_id;
        fin >> trial_id;
        fin.close();
        std::string event_file = m_events_dir + "/" + trial_id + ".csv";

        std::string row = trial_id  + "," + m_selection + "," + m_technique + "," + std::to_string(m_no_of_grids);

        row             = row + "," + std::to_string(_target_id)                + ",\"("  + std::to_string(_target_loc_indices.first)  + "," + std::to_string(_target_loc_indices.second) + ")\"";

        row             = row + "," + std::to_string(_time_taken_ms)            + ","     + std::to_string(_dist_traveled_px);

        row             = row + "," + std::to_string(_attempts)                 + ","     + std::to_string(_visited_cells); 

        row             = row + "," + std::to_string(_dist_travelled_left_hand) + ","     + std::to_string(_dist_travelled_right_hand);

        row             = row + "," + _lefthand_landmarks                       + ","     + _righthand_landmarks;

        save_event_row(event_file, row);
    }
}