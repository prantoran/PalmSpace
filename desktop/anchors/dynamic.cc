#include <string>
#include <iostream>

#include "anchors.h"


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
  m_type = choices::anchor::DYNAMIC;
  
  width = 0;
  height = 0;

  m_grid.reset_dimensions();
  m_grid_out.reset_dimensions();
  
  color_green = COLORS_darkgreen;

  m_selected_i_prv = -1, m_selected_j_prv = -1;
  m_selected_i = -1, m_selected_j = -1;
  green_i = -1, green_j = -1;

  m_static_display = false;

  palmbase_x = 0;
  palmbase_y = 0;

  indexbase_x = 0;
  indexbase_y = 0;

  m_selection_locked = false;

  reset_grids();
  reset_palmbase();
  reset_indexbase();
}


void AnchorDynamic::calculate(
    const cv::Mat& input, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {
    


    if (!width || !height) {
      setConfig(input.size().width, input.size().height);
      m_screen.setMinWidthHeight(
        m_grid.m_width_min, m_grid.m_height_min, 
        width, height);
      m_screen.setMinWidthHeight(
        m_grid_out.m_width_min, m_grid_out.m_height_min, 
        width, height);
      
    }

    // m_grid.m_width_min = params.palm_width();
    // m_grid.m_height_min = params.palm_height();

    // m_grid_out.m_width_min = params.palm_width();
    // m_grid_out.m_height_min = params.palm_height();

    updateBase(params.m_palmbase, palmbase_x, palmbase_y);
    updateBase(params.m_indexbase, indexbase_x, indexbase_y);

    if (indexbase_x > 0 && indexbase_y > 0) {
      // using palmbase
      // setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
      // using indexbase
      
      m_grid.align(indexbase_x*width, indexbase_y*height);
      m_grid_out.align(indexbase_x*width, indexbase_y*height);
      
      setupSelection(pointer_x, pointer_y, m_selected_i, m_selected_j); // defined in parent anchor class

      params.set_selected_cell(m_selected_i, m_selected_j);
    }
}


void AnchorDynamic::updateBase(const SmoothCoord & base, double & _x, double & _y) {
  _x = base.x();
  _y = base.y();
}


void AnchorDynamic::draw(
    const cv::Mat& input, 
    cv::Mat& output, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {

    cv::Mat overlay;
    input.copyTo(overlay);

    draw_main_grid_layout(overlay, m_grid_out);
    
    draw_cells(overlay, m_grid_out);

    drawTextHighlighted(overlay);
    drawTextSelected(overlay);

    drawProgressBar(overlay, params);
    
    cv::addWeighted(overlay, alpha, input, 1-alpha, 0, output);
}



