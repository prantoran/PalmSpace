#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <tuple>

// opencv
#include "mediapipe/framework/port/opencv_imgproc_inc.h"


#include "choices.h"
#include "../camera/camera.h"

const std::string APP_NAME = "PalmSpace";

constexpr int PSIZE = 10;


class DepthArea {
  public:
  int area1, area2;
  cv::Point pt1, pt2;
  bool initiated;

  DepthArea() {
    initiated = false;
  }
};


class Range{
    public:
    using number_t = int; // this is a nested type alias
    number_t m_upper_bound, m_lower_bound;
    
    Range();
    Range(number_t _lb, number_t _ub);
    bool isInside(number_t val);
    // TODO use << overloading for print
    void print();
};



// TODO refactor to cache
class ExtraParameters {
  // TODO add m_ prefix to all attributes

  int total_hands;

  // stores the indexes of the grid's cell which is currently selected
  // mainly used by trigger::dwell and anchors
  std::tuple<int, int> selected_cell;
  std::tuple<double, double, double> m_indexbase, m_palmbase;
  std::tuple<double, double, double> m_primary_cursor, m_primary_cursor_middlefinger_base;
  Camera * m_camera;



  public:
  using index_t = int;

  index_t m_cursor_id, m_cursor_middlebase_id;

  int m_frame_width, m_frame_height;
  
  // TODO safely remove extra_params vector
  std::vector<double> extra_params;
  int psize;

  // these are tuples from MediaPipe, NOT OpenCV Mat indices

  bool load_video; // used by trigger tap_depth_area

  DepthArea depth_area; // for area based depth calculation

  bool is_static_display;

// https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160
// When initialization must be separate from declaration, e.g. class members, 
// initialize with nullptr to make your programming intent explicit.
  cv::Mat* depth_mat;
  float medians[25];
  /*
    0: min_ws
    1: min_hs
    2: palmbase_x
    3: palmbase_y
    4: otherindex_x
    5: otherindex_y
    6: otherindex_z
    7: selected_i / row
    8: selected_j / col
    9: progress_bar% [0-100]
  */

  int m_row, m_col;

  Range m_flood_width, m_flood_height;
  
  ExtraParameters();
  // TODO add camera argument in constructor
  ExtraParameters(int _frame_width, int _frame_height, bool _load_video, Camera * _camera);
  ~ExtraParameters();

  void init(bool _load_video);
  void reset();

  void set(int i, double v);
  void set(const std::vector<double> & p);

  double at(int i);

  void get_palmbase(double &x, double &y);
  void get_palmbase(std::tuple<double, double, double> & p);
  void set_palmbase(const std::tuple<double, double, double> & p);
  void set_palmbase(double x, double y);

  void get_indexbase(double &x, double &y);
  void get_indexbase(std::tuple<double, double, double> & p);
  void get_indexbase_cv_indices(index_t & rowi, index_t & colj);
  void set_indexbase(const std::tuple<double, double, double> & p);
  void set_indexbase(double x, double y);

  void get_palmbase_middle_cv_indices(index_t &x_col, index_t &y_row);

  void set_is_static(bool is_static);
  bool is_static();

  int total_hands_detected() const;
  void set_total_hands(int hands);

  void get_selected_cell(int &row_i, int &col_j);
  void set_selected_cell(index_t row_i, index_t col_j);

  void set_depth_map(cv::Mat * _mat);
  void get_depth_at(
    const std::vector<int> & rows, 
    const std::vector<int> & cols, 
    double & value);

  void set_primary_cursor(const std::tuple<double, double, double> & p);
  void set_primary_cursor(double x, double y);
  void get_primary_cursor(double & x, double & y);
  void get_primary_cursor_cv_indices(index_t & x_col, index_t & y_row);
  bool is_set_primary_cursor();

  void set_primary_cursor_middlefinger_base(const std::tuple<double, double, double> & p);
  void get_primary_cursor_middlefinger_base_cv_indices(index_t & x_col, index_t & y_row);

  float get_depth(int x_col, int y_row);

  void get_cv_indices(
    const std::tuple<double, double, double> & point, 
    index_t & x_col, index_t & y_row);

};

#endif