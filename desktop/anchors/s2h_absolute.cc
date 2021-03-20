
#include "s2h_absolute.h"


AnchorS2HAbsolute::~AnchorS2HAbsolute() {
  std::cout << "anchor s2h-absolute killed\n";
}


AnchorS2HAbsolute::AnchorS2HAbsolute() {
  initiate();
  color_red = cv::Scalar(25, 25, 255);
  color_blue = cv::Scalar(255, 25, 25);
}


AnchorS2HAbsolute::AnchorS2HAbsolute(
  const cv::Scalar & red, 
  const cv::Scalar & blue) {
  
  initiate();
  color_red = red;
  color_blue = blue;
}

 
void AnchorS2HAbsolute::initiate() {
  name = "s2h_absolute";
  m_type = choices::anchor::S2HABSOLUTE;
  
  width = 0;
  height = 0;

  m_grid.reset_dimensions();
  m_grid_out.reset_dimensions();
  
  color_green = COLORS_darkgreen;

  reset_selection();
  
  reset_marked_cell();

  m_static_display = false;

  palmbase_x = 0;
  palmbase_y = 0;

  indexbase_x = 0;
  indexbase_y = 0;

  m_selection_locked = false;

  reset_grids();
  reset_palmbase();
  reset_palmbase_right();
  reset_indexbase();

  m_visited_cells = 0;

  m_absolute = false;
}


void AnchorS2HAbsolute::calculate(
  const cv::Mat& input, 
  double scale_ratio, 
  int pointer_x, int pointer_y,
  Parameters & params) {
  
  
  m_calculate_done = false;

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

  if (!m_absolute) {
    
    if (m_inputspace_type == choices::inputspace::PALMSIZED) {
      m_palm_x = params.palm_width();
      m_palm_y = params.palm_height();

      m_max_dim = std::max(m_palm_x.second  - m_palm_x.first, m_palm_y.second - m_palm_y.first);
      
      if (m_max_dim < 110) {
        initiate();
        return;
      }

      m_grid.m_width_min  = std::max(64.0, std::min((double)params.m_frame_width, m_max_dim));
      m_grid.m_height_min = std::max(64.0, std::min((double)params.m_frame_height, m_max_dim));

      m_grid_out.m_width_min = m_grid.m_width_min;
      m_grid_out.m_height_min = m_grid.m_height_min;

    }

    updateBase(params.m_palmbase, palmbase_x, palmbase_y);
    updateBase(params.m_indexbase, indexbase_x, indexbase_y);
  }

  if (!m_absolute && indexbase_x > 0 && indexbase_y > 0) {
    m_absolute = true;
    m_static_display = true;
    // using palmbase
    // setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
    // using indexbase

    indexbase_x *= width;
    indexbase_y *= height;

    if (indexbase_x + m_grid.m_width_min + 160 >= width) 
      indexbase_x = width - m_grid.m_width_min - 160;
    if (indexbase_y + m_grid.m_height_min + 80 >= height) 
      indexbase_y = height - m_grid.m_height_min - 80;
  }

  if (m_absolute) {
    // std::cerr << "widht:" << width  << "\theight:" << height << "\tinde"
    m_grid.align(indexbase_x, indexbase_y);
    m_grid_out.align(indexbase_x, indexbase_y);
    
    setupSelection(pointer_x, pointer_y, m_selected_i, m_selected_j); // defined in parent anchor class

    params.set_selected_cell(m_selected_i, m_selected_j);
  }

  m_calculate_done = true;
}


void AnchorS2HAbsolute::updateBase(const SmoothCoord & base, double & _x, double & _y) {
  _x = base.x();
  _y = base.y();
}


void AnchorS2HAbsolute::draw(
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
    drawTextMarked(overlay);

    drawProgressBar(overlay, params);
    
    cv::addWeighted(overlay, TRANSPARENCY_ALPHA, input, 1-TRANSPARENCY_ALPHA, 0, output);
}



