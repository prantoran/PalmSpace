#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <tuple>
#include <utility>

// opencv
#include "mediapipe/framework/port/opencv_imgproc_inc.h"


#include "choices.h"
#include "grid.h"
#include "coord.h"
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


namespace handedness {
  typedef enum {
    LEFT,
    RIGHT,
    UNDEFINED,
  } hand;
}


class Parameters {
  // TODO add m_ prefix to all attributes

  int m_total_hands;

  // stores the indexes of the grid's cell which is currently selected
  // mainly used by trigger::dwell and anchors
  std::tuple<int, int> selected_cell;

  
  Camera * m_camera;


  std::pair<double, double> m_raw_dimensions;
  double m_progress_bar;

  std::tuple<double, double, double> m_other_index;

  public:
  SmoothCoord m_indexbase, m_palmbase;
  SmoothCoord m_primary_cursor, m_primary_cursor_middlefinger_base;
  
  using index_t = int;

  index_t m_cursor_id, m_cursor_middlebase_id;

  int m_frame_width, m_frame_height;
  
  // these are tuples from MediaPipe, NOT OpenCV Mat indices

  bool load_video; // used by trigger tap_depth_area

  DepthArea depth_area; // for area based depth calculation

  bool is_static_display;

  // https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160
  // When initialization must be separate from declaration, e.g. class members, 
  // initialize with nullptr to make your programming intent explicit.
  cv::Mat* depth_mat;
  float medians[25];

  Range m_flood_width, m_flood_height;

  bool m_show_depth_txt;
  std::string m_depth_txt;

  int m_base_id; // m_base_hand_id_set_by_initiator;

  handedness::hand hand[4];
  int m_hand_mask;

  std::pair<double, double> m_hand_landmarks_relative_depth_minmax[4];
  int m_hand_color_scale[4], m_hand_size_scale[4];

  // std::vector<std::vector<std::tuple<double, double, double>>> m_points;
  std::vector<std::vector<SmoothCoord>> m_points;

  bool m_selection_changed;

  void get_raw_dimensions(double & _width, double _height);
  void set_raw_dimensions(double _width, double _height);
  
  Parameters();
  Parameters(int _frame_width, int _frame_height, bool _load_video, Camera * _camera);
  ~Parameters();

  void init(bool _load_video);
  void reset();
  
  void get_palmbase(double &x_col, double &y_row);
  void get_palmbase(std::tuple<double, double, double> & p);
  void set_palmbase(const std::tuple<double, double, double> & p);
  // void set_palmbase(double x_col, double y_row);

  void get_indexbase(double &x_col, double &y_row);
  void get_indexbase(std::tuple<double, double, double> & p);
  void set_indexbase(const std::tuple<double, double, double> & p);
  // void set_indexbase(double x_col, double y_row);


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
  cv::Point cursor_cvpoint();
  
  void set_primary_cursor_middlefinger_base(const std::tuple<double, double, double> & p);
  void get_primary_cursor_middlefinger_base_cv_indices(index_t & x_col, index_t & y_row);

  float get_depth(int x_col, int y_row);

  void get_cv_indices(
    SmoothCoord & point, 
    index_t & x_col, index_t & y_row);

  void set_progress_bar(double _progress);
  void get_progress_bar(double & _progress);


  void set_other_index(); // TODO need more inspection
  void get_other_index_z_value(double _z);

  int cursor_hand_id();

  int primary_cursor_size();
  int primary_cursor_color_size();

  std::pair<double, double> palm_width() const;
  std::pair<double, double> palm_height() const;
  
  cv::Point thumb_tip();
  cv::Point index_tip();
  cv::Point thumb_base();

  std::vector<std::vector<std::tuple<double, double, double>>> get_points();
};



#endif