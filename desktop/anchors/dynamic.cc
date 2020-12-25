#include "anchors.h"
#include <string>
#include <iostream>


AnchorDynamic::~AnchorDynamic() {
  std::cout << "anchor dynamic killed\n";
}

AnchorDynamic::AnchorDynamic() {
  initiate();
  color_red = cv::Scalar(25, 25, 255);
  color_blue = cv::Scalar(255, 25, 25);
}

AnchorDynamic::AnchorDynamic(
  const cv::Scalar & red, 
  const cv::Scalar & blue) {
  
  initiate();
  color_red = red;
  color_blue = blue;
}


void AnchorDynamic::initiate() {
  name = "dynamic";
  _type = choices::anchor::DYNAMIC;
  
  width = 0;
  height = 0;
  momentum = 0.9;
  gap = 5;

  ws = 0;
  hs = 0;
  
  color_green = COLORS_darkgreen;

  selected_i_prv = -1, selected_j_prv = -1;
  selected_i = -1, selected_j = -1;
  green_i = -1, green_j = -1;

  static_display = false;

  palmbase_x = 0;
  palmbase_y = 0;

  indexbase_x = 0;
  indexbase_y = 0;


  reset_grid();
  reset_palmbase();
  reset_indexbase();
}


void AnchorDynamic::calculate(
    const cv::Mat& input, 
    const std::tuple<double, double, double> & palmbase,
    const std::tuple<double, double, double> & indexbase,
    double scale_ratio, 
    int pointer_x, int pointer_y,
    ExtraParameters & params) {
    
    std::vector<double> & extra_params = params.extra_params; 

    if (!width || !height) {
      setConfig(input.size().width, input.size().height);
      
      if (extra_params.size() >= 2 && extra_params[0] != -1 && extra_params[1] != -1) {
        // extra parameters, for eg from initiator
        min_ws = extra_params[0]*width;
        min_hs = extra_params[1]*height;
      } {
        screen.setMinWidthHeight(min_ws, min_hs, width, height);
      }
    }

    updatePalmBase(palmbase);
    updateIndexBase(indexbase);

    if (indexbase_x > 0 && indexbase_y > 0) {
      // using palmbase
      // setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
      // using indexbase
      setupGrid(indexbase_x*width, indexbase_y*height); // defined in parent anchor class
      setupSelection(pointer_x, pointer_y, selected_i, selected_j); // defined in parent anchor class

      params.set_selected_cell(selected_i, selected_j);
    }
}

void AnchorDynamic::updatePalmBase(const std::tuple<double, double, double> & palmbase) {
  palmbase_x_prv = palmbase_x;
  palmbase_y_prv = palmbase_y;

  palmbase_x = std::get<0>(palmbase);
  palmbase_y = std::get<1>(palmbase);
  
  if (palmbase_x_prv > 0) {
    palmbase_x = (1-momentum)*palmbase_x + momentum*palmbase_x_prv;
  }
  
  if (palmbase_y_prv > 0) {
    palmbase_y = (1-momentum)*palmbase_y + momentum*palmbase_y_prv;
  }
}


void AnchorDynamic::updateIndexBase(const std::tuple<double, double, double> & indexbase) {
  indexbase_x_prv = indexbase_x;
  indexbase_y_prv = indexbase_y;

  indexbase_x = std::get<0>(indexbase);
  indexbase_y = std::get<1>(indexbase);
  
  if (indexbase_x_prv > 0) {
    indexbase_x = (1-momentum)*indexbase_x + momentum*indexbase_x_prv;
  }
  
  if (indexbase_y_prv > 0) {
    indexbase_y = (1-momentum)*indexbase_y + momentum*indexbase_y_prv;
  }
}


void AnchorDynamic::draw(
    const cv::Mat& input, 
    cv::Mat& output, 
    const std::tuple<double, double, double> & palmbase,
    const std::tuple<double, double, double> & indexbase, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    const ExtraParameters & params) {

    cv::Mat overlay;
    input.copyTo(overlay);

    cv::rectangle(
        overlay, 
        getGridTopLeft(), getGridBottomRight(), 
        COLORS_floralwhite,
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

    drawProgressBar(overlay, params.extra_params[9]);
    
    cv::addWeighted(overlay, alpha, input, 1-alpha, 0, output);
}


