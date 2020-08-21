#include "anchors.h"
#include <string>
#include <iostream>
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

std::string AnchorDynamic::type() {
    return "anchor-type=dynamic";
}

AnchorDynamic::AnchorDynamic() {
  name = "dynamic";
  
  width = 0;
  height = 0;
  palmbase_momentum = 0.9;
  gap = 5;

  color_red = cv::Scalar(25, 25, 255);
  color_blue = cv::Scalar(255, 25, 25);


  selected_i_prv = -1, selected_j_prv = -1;
  selected_i = -1, selected_j = -1;

  static_display = false;
}

AnchorDynamic::AnchorDynamic(cv::Scalar red, cv::Scalar blue) {
  width = 0;
  height = 0;
  palmbase_momentum = 0.9;
  gap = 5;

  color_red = red;
  color_blue = blue;
  color_green = cv::Scalar(25, 255, 25);

  selected_i_prv = -1, selected_j_prv = -1;
  selected_i = -1, selected_j = -1;
  static_display = false;

  palmbase_x = 0;
  palmbase_y = 0;

  green_i = -1, green_j = -1;
}



void AnchorDynamic::calculate(
    const cv::Mat& input, 
    double palmbase_x_new, double palmbase_y_new, 
    double interface_scaling_factor, 
    int pointer_x, int pointer_y,
    std::vector<double> & extra_params) {

    if (!width || !height) {
      setConfig(input.size().width, input.size().height);
    }
    
    if (extra_params.size() >= 2 && extra_params[0] != -1 && extra_params[1] != -1) {
      // extra parameters, for eg from initiator
      min_ws = extra_params[0]*width;
      min_hs = extra_params[1]*height;
    } else {
      min_ws = width/3;
      min_hs = height/3;
    }

    palmbase_x_prv = palmbase_x;
    palmbase_y_prv = palmbase_y;

    palmbase_x = palmbase_x_new;
    palmbase_y = palmbase_y_new;
    
    // std::cout << "dx:" << dx << " dy:" << dy << "\n";
        // std::cout << "1dx:" << dx << " dy:" << dy << "\n";

    if (palmbase_x_prv) {
      palmbase_x = (1-palmbase_momentum)*palmbase_x + palmbase_momentum*palmbase_x_prv;
    }
    
    if (palmbase_y_prv) {
      palmbase_y = (1-palmbase_momentum)*palmbase_y + palmbase_momentum*palmbase_y_prv;
    }
        std::cerr <<"using palmbase_x_new:" << palmbase_x_new << " palmbase_y_new:" << palmbase_y_new << "\n";

    setupGrid(); // defined in parent anchor class
    
    // std::cerr << "anchor palmbase_x:" << palmbase_x << " width:" << width << " ws:" << ws << " hs:" << hs << " dx:" << dx << " dy:" << dy << " gap:" << gap <<" divisions:" << divisions <<"\n";
    // std::cerr  << " (ws - (divisions+1)*gap):" << (ws - (divisions+1)*gap) << " (hs - (divisions+1)*gap):" << (hs - (divisions+1)*gap) << "\n";

    setupSelection(pointer_x, pointer_y); // defined in parent anchor class

    std::cout << "anchors_dynamic selected i:" << selected_i << "\tj:" << selected_j << "\n";
    if (extra_params.size() > 7) {
      std::cout << "\tanchors_dynamic storing selected i:" << selected_i << "\tj:" << selected_j << "\n";

      extra_params[7] = selected_i;
      extra_params[8] = selected_j;
    } else {
      std::cerr << "anchors_dynamic extra_params small size, cannot store selected i-j\n";
    }

}

void AnchorDynamic::draw(
    cv::Mat& input, 
    double palmbase_x_new, double palmbase_y_new, 
    double interface_scaling_factor, 
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
    
    std::cout << "anchor_dynamic draw selected i:" << selected_i << " j:" << selected_j << "\n";

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

    // std::cerr << "anchors_dynamic message:" << message << "\n";
    
    drawTextHighlighted(overlay);
    drawTextSelected(overlay);

    drawProgressBar(overlay, extra_params[9]);
    
    cv::addWeighted(overlay, alpha, input, 1-alpha, 0, input);
}

void AnchorDynamic::reset_palmbase() {
    palmbase_x = 0;
    palmbase_y = 0;
    ws = 0;
    hs = 0;
}


