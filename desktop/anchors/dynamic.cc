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

  m_grid.m_width = 0;
  m_grid.m_height = 0;
  
  color_green = COLORS_darkgreen;

  m_selected_i_prv = -1, m_selected_j_prv = -1;
  m_selected_i = -1, m_selected_j = -1;
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
    Parameters & params) {
    
    if (!width || !height) {
      setConfig(input.size().width, input.size().height);
      screen.setMinWidthHeight(
        m_grid.m_width_min, m_grid.m_height_min, 
        width, height);
    }

    updatePalmBase(palmbase);
    updateIndexBase(indexbase);

    if (indexbase_x > 0 && indexbase_y > 0) {
      // using palmbase
      // setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
      // using indexbase
      setupGrid(indexbase_x*width, indexbase_y*height); // defined in parent anchor class
      setupSelection(pointer_x, pointer_y, m_selected_i, m_selected_j); // defined in parent anchor class

      params.set_selected_cell(m_selected_i, m_selected_j);
    }
}

void AnchorDynamic::updatePalmBase(const std::tuple<double, double, double> & palmbase) {
  palmbase_x = std::get<0>(palmbase);
  palmbase_y = std::get<1>(palmbase);
}


void AnchorDynamic::updateIndexBase(const std::tuple<double, double, double> & indexbase) {
  indexbase_x = std::get<0>(indexbase);
  indexbase_y = std::get<1>(indexbase);
}


void AnchorDynamic::draw(
    const cv::Mat& input, 
    cv::Mat& output, 
    const std::tuple<double, double, double> & palmbase,
    const std::tuple<double, double, double> & indexbase, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {

    cv::Mat overlay;
    input.copyTo(overlay);

    cv::rectangle(
        overlay, 
        getGridTopLeft(), getGridBottomRight(), 
        COLORS_floralwhite,
        -1, 
        cv::LINE_8,
        0);
    

    for (int i = 1; i <= m_grid.m_divisions; i ++ ) {
        for (int j = 1; j <= m_grid.m_divisions; j ++) {
          color_cur = color_red;
          if (i == green_i && j == green_j) {
            color_cur = color_green;
          } else if (i == m_selected_i && j == m_selected_j) {
            color_cur = color_blue;
          }

          cv::rectangle(
            overlay, 
            m_grid.get_cell(i, j),
            color_cur,
            -1, 
            cv::LINE_8,
            0);
        }
    }

    drawTextHighlighted(overlay);
    drawTextSelected(overlay);

    drawProgressBar(overlay, params);
    
    cv::addWeighted(overlay, alpha, input, 1-alpha, 0, output);
}



