
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
        bool ancdyn, ancstat, ancmid; 
        bool trigpalmbase, trigpalmfree, trigpinch, trigwait, trigtap, trigdwell, trigtapdepth, trigtapdepthsingle;

        // eScreenSize screen_size;

        bool screen_small, screen_large, screen_full;

        int scalex, scaley; // to adjust topleft position of frame

        std::string errormsg;

        bool valid;

        bool visibility_fixed, visibility_conditional;

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
            int & depth);
    };


    void button(cv::Mat & frame, const std::string & label, bool & state);
};



#endif