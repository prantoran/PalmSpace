#include "triggers.h"

// opencv
#include "mediapipe/framework/port/opencv_highgui_inc.h" // GUI #include "opencv2/highgui/highgui.hpp"


void setCyanThreshold(int & minR, int & maxR, int & minG, int & maxG, int & minB, int & maxB) {
    minR = 0, maxR = 110;
    minG = 0, maxG = 255;
    minB = 171, maxB = 255;
}

TriggerTapDepthArea::TriggerTapDepthArea() {}


TriggerTapDepthArea::TriggerTapDepthArea(
    const bool save_video, 
    const bool load_video, 
    const int opt_dev_video, 
    const int fps,
    const int frame_width,
    const int frame_height) {
    cv::namedWindow(trackbarWindowName, cv::WINDOW_AUTOSIZE);

  // int minH = 130, maxH = 160, minS = 10, maxS = 40, minV = 75, maxV = 130;

    // cv::createTrackbar("MinH", trackbarWindowName, &minH, 239);
    // cv::createTrackbar("MaxH", trackbarWindowName, &maxH, 239);
    // cv::createTrackbar("MinS", trackbarWindowName, &minS, 255);
    // cv::createTrackbar("MaxS", trackbarWindowName, &maxS, 255);
    // cv::createTrackbar("MinV", trackbarWindowName, &minV, 255);
    // cv::createTrackbar("MaxV", trackbarWindowName, &maxV, 255);

    minR = 233, maxR = 255, minG = 233, maxG = 255, minB = 233, maxB = 255;
    // minR2 = 233, maxR2 = 255, minG2 = 59, maxG2 = 149, minB2 = 95, maxB2 = 186;
    setCyanThreshold(minR2, maxR2, minG2, maxG2, minB2, maxB2);

    cv::createTrackbar("MinR", trackbarWindowName, &minR, 255);
    cv::createTrackbar("MaxR", trackbarWindowName, &maxR, 255);
    cv::createTrackbar("MinG", trackbarWindowName, &minG, 255);
    cv::createTrackbar("MaxG", trackbarWindowName, &maxG, 255);
    cv::createTrackbar("MinB", trackbarWindowName, &minB, 255);
    cv::createTrackbar("MaxB", trackbarWindowName, &maxB, 255);
    
    
    cv::VideoCapture capture;
    if (load_video) {
        // capture.open(input_video_path); // read a video file
    } else {
        capture = cv::VideoCapture(opt_dev_video);
        capture.open(0);
        if (!capture.isOpened()) {
            std::cout << "ERROR: trigger_tap_depth_area: camera could not be opened\n";
            return;
        };
    }

    if (!save_video) {
        cv::namedWindow(trackbarWindowName, cv::WINDOW_AUTOSIZE);
        if (!load_video) {
            #if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
                capture.set(cv::CAP_PROP_FRAME_WIDTH, frame_width);
                capture.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height);
                capture.set(cv::CAP_PROP_FPS, fps);
            #endif
        }
    }

    cv::Mat camera_frame_raw;

    while (true) {
        capture >> camera_frame_raw;
        if (camera_frame_raw.empty()) break;  // End of video.
            cv::cvtColor(camera_frame_raw, hsv, cv::COLOR_BGR2RGB);
        if (!load_video) {
            cv::flip(hsv, hsv, 1);
        }

        cv::inRange(hsv, cv::Scalar(minR, minG, minB), cv::Scalar(maxR, maxG, maxB), hsv);

        // int blurSize = 3;
        // int elementSize = 3;
        // cv::medianBlur(hsv, hsv, blurSize);
        // cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));
        // cv::dilate(hsv, hsv, element);


        cv::imshow(trackbarWindowName, hsv);
        if (cv::waitKey(30) >= 0) break;
    }

    cv::destroyWindow(trackbarWindowName);
}


void TriggerTapDepthArea::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    ExtraParameters & params) {


    cv::cvtColor(input_image, hsv, cv::COLOR_BGR2RGB);
    cv::cvtColor(input_image, hsv2, cv::COLOR_BGR2RGB);

    if (!params.load_video) {
        cv::flip(hsv, hsv, 1);
        cv::flip(hsv2, hsv2, 1);
    }

    // cv::inRange(hsv, cv::Scalar(minH, minS, minV), cv::Scalar(maxH, maxS, maxV), hsv);
    cv::inRange(hsv, cv::Scalar(minR, minG, minB), cv::Scalar(maxR, maxG, maxB), hsv);
    cv::inRange(hsv2, cv::Scalar(minR2, minG2, minB2), cv::Scalar(maxR2, maxG2, maxB2), hsv2);

    cv::Moments m1 = cv::moments(hsv);
    cv::Moments m2 = cv::moments(hsv2);

    cv::Point pt1(m1.m10/m1.m00, m1.m01/m1.m00);
    cv::Point pt2(m2.m10/m2.m00, m2.m01/m2.m00);

    // std::cerr << "centroid 1: (" << pt1.x << "," << pt1.y << ") centroid 2: (" << pt2.x << ", " << pt2.y << ")\n";

    int area1 = cv::countNonZero(hsv);
    int area2 = cv::countNonZero(hsv2);

    params.depth_area.initiated = true;
    params.depth_area.area1 = area1;
    params.depth_area.area2 = area2;
    params.depth_area.pt1 = pt1;
    params.depth_area.pt2 = pt2;

    std::cerr << "area1:" << area1 << " area2:" << area2 << "\n";


    cv::addWeighted( hsv, 1, hsv2, 1, 0.0, hsv);
    cv::imshow(trackbarWindowName, hsv);
}


TRIGGER::state TriggerTapDepthArea::status() {
    return cur_state;
}