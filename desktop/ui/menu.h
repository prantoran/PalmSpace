
#ifndef MENU_H
#define MENU_H

#include "desktop/config/choices.h";


#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

#include <string>

namespace PalmSpaceUI {

    class Menu {
        int width, height;
        std::string _window_name;
        bool _debug;
        bool _depth;

        cv::Mat frame;
        int low_threshold, high_threshold;
        int cellcnt;
        bool onehand, twohand;
        bool ancdyn, ancstat, ancmid, anchandtoscreen; 
        bool trigpalmbase, trigpalmfree, trigpinch, trigtap, 
            trigdwell, trigtapdepth, trigtapdepthsingle, 
            trigdepthdistance;

        // eScreenSize screen_size;

        bool screen_small, screen_large, screen_full;

        int scalex, scaley; // to adjust topleft position of frame

        std::string errormsg;

        bool visibility_fixed, visibility_conditional;

        bool trial_start_btn_location_left, 
            trial_start_btn_location_center, 
            trial_start_btn_location_left_center,
            trial_start_btn_location_right_center;

        bool m_trial_pause_before_each_target;
        bool m_trial_show_button_during_trial;

        public:
        
        Menu(
            int frame_width, 
            int frame_height,
            int anchor, 
            int trigger, 
            int initiator,
            int divisions, 
            int screensize,
            int visibility,
            bool debug, 
            bool use_depth,
            int trial_start_btn_location,
            bool trial_pause_before_each_target,
			bool trial_show_button_during_trial,
            std::string window_name
        );

        void run();
        
        void get_choices(
            int & initiator, 
            int & anchor, 
            int & trigger, 
            int & divisions,
            int & screensize,
            int & visibility,
            int & debug,
            int & depth,
            int & trial_start_btn_location,
            bool & trial_pause_before_each_target,
            bool & trial_show_button_during_trial);

        bool is_valid();
    };


    void button(cv::Mat & frame, const std::string & label, bool & state);
};



#endif