#ifndef GRID_H
#define GRID_H

#include "mediapipe/framework/port/opencv_imgproc_inc.h"

class Grid {
  public:
  int m_divisions;
  double m_width, m_height, m_width_min, m_height_min;
  double m_gap, m_x_cols[11], m_y_rows[11], m_dx_col, m_dy_row;
  bool m_dynamic_dimensions;
  Grid();
  void reset_minimum_dimensions();
  void reset_dimensions();
  cv::Point get_bottom_right() const;
  cv::Point get_top_left() const;
  cv::Point get_bottom_left() const;
  cv::Point get_top_right() const;

  cv::Rect get_bound_rect() const;
  void reset();
  int arg_x(int pointer_x) const;
  int arg_y(int pointer_y) const;
  void align(double topleft_x, double topleft_y);
  cv::Rect get_cell(int i, int j) const;
  void get_center_cv(int & _col_width, int & _row_height);
  bool is_inside_cv(int _col_x, int _row_y) const;
};


#endif