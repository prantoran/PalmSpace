
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

        cv::Mat frame;
        int low_threshold, high_threshold;
        int cellcnt;
        bool onehand, twohand;
        bool ancdyn, ancstat, ancmid; 
        bool trigpalmbase, trigpalmfree, trigpinch, trigwait, trigtap, trigdwell;

        // eScreenSize screen_size;

        bool screen_small, screen_large, screen_full;

        int scalex, scaley; // to adjust topleft position of frame

        std::string errormsg;

        bool valid;


        public:
        Menu(
            int FLAGS_frame_width, int FLAGS_frame_height,
            int choice_anchor, int choice_trigger, int choice_initiator,
            int choice_divisions, int choice_screensize,
            bool FLAGS_debug, std::string window_name
        );
        void run();
        void get_choices(
            int & initiator, 
            int & anchor, 
            int & trigger, 
            int & divisions,
            int & screensize,
            int & debug);
    };
};



#endif