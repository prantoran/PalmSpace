
#ifndef MENU_H
#define MENU_H



#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

#include <string>



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

    int scalex, scaley; // to adjust topleft position of frame

    std::string errormsg;

    bool valid;

    public:
    Menu(
        int FLAGS_frame_width, int FLAGS_frame_height, 
        int choice_divisions, bool FLAGS_debug, std::string window_name);
    void run();
    void get_choices(
        int & initiator, 
        int & anchor, 
        int & trigger, 
        int & divisions,
        int & debug);
};


#endif