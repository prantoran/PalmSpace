#include "anchors.h"
 
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

#include <vector>

std::string AnchorMidAir::type() {
    return "anchor-type=dynamic";
}

AnchorMidAir::~AnchorMidAir() {
    std::cout << "anchor midair killed\n";
}

AnchorMidAir::AnchorMidAir() {
    name = "static";

    width = 0;
    height = 0;
    gap = 15;

    color_red = cv::Scalar(25, 25, 255);
    color_blue = cv::Scalar(255, 25, 25);
    color_green = cv::Scalar(25, 255, 25);

    selected_i_prv = -1, selected_j_prv = -1;
    selected_i = -1, selected_j = -1;

    static_display = false;
}

AnchorMidAir::AnchorMidAir(cv::Scalar red, cv::Scalar blue) {
    width = 0;
    height = 0;
    gap = 5;

    color_red = red;
    color_blue = blue;
    color_green = cv::Scalar(25, 255, 25);

    selected_i_prv = -1, selected_j_prv = -1;
    selected_i = -1, selected_j = -1;
    static_display = false;
}

void AnchorMidAir::calculate(
    const cv::Mat& input, 
    const std::tuple<double, double, double> & palmbase,
    const std::tuple<double, double, double> & indexbase, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    std::vector<double> & extra_params) {


    if (!width || !height) {
        setConfig(input.size().width, input.size().height);

        min_ws = width;
        min_hs = height;
    }
    
    if (std::get<0>(palmbase) >= 0 && std::get<1>(palmbase) >= 0) {
        palmbase_x = 0.5; // determines a position of grid
        palmbase_y = 1;   // middle-bottom point

        static_display = true;
    }

    setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
        
    std::cout << "anchor_midair pointer x:" << pointer_x << " y:" << pointer_y << "\n";
    setupSelection(pointer_x, pointer_y); // defined in parent anchor class

    std::cout << "anchor_midair selected i:" << selected_i << "\tj:" << selected_j << "\n";

    if (extra_params.size() > 7) {
      extra_params[7] = selected_i;
      extra_params[8] = selected_j;
    } else {
      std::cerr << "anchor_midair extra_params small size, cannot store selected i-j\n";
    }

}

void AnchorMidAir::draw(
    cv::Mat& input, 
    const std::tuple<double, double, double> & palmbase,
    const std::tuple<double, double, double> & indexbase, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    std::vector<double> & extra_params) {

    cv::Mat overlay;
    input.copyTo(overlay);

    cv::rectangle(
        overlay, 
        cv::Point(xs[0], ys[0]), cv::Point(xs[0]+ws, ys[0]+hs), 
        cv::Scalar(25, 25, 125),
        -1, 
        cv::LINE_8,
        0);

    for (int i = 1; i <= divisions; i ++ ) {
        for (int j = 1; j <= divisions; j ++) {
          color_cur = color_red;
          if (i == green_i && j == green_j) {
            color_cur = color_green;
          } else if (i == selected_i && j == selected_j) {
            color_cur = color_blue;
          }

          cv::rectangle(
            overlay, 
            cv::Point(xs[i], ys[j]), cv::Point(xs[i]+dx, ys[j]+dy), 
            color_cur,
            -1, 
            cv::LINE_8,
            0);
        }
    }

    drawTextHighlighted(overlay);
    drawTextSelected(overlay);

    drawProgressBar(overlay, extra_params[9]);

    cv::addWeighted(overlay, alpha, input, 1-alpha, 0, input);
}
