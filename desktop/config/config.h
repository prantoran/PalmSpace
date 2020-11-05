#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <tuple>

// opencv
#include "mediapipe/framework/port/opencv_imgproc_inc.h"

#include "choices.h"

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


class ExtraParameters {
    public:
    std::vector<double> extra_params;
    int psize;

    std::tuple<double, double, double> indexbase, palmbase;
    
    bool load_video; // used by trigger tap_depth_area

    DepthArea depth_area; // for area based depth calculation
    
    bool is_static_display;

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

    ExtraParameters();
    ExtraParameters(bool _load_video);
    ~ExtraParameters();

    void init(bool _load_video);
    void reset();
    
    void set(int i, double v);
    void set(const std::vector<double> & p);

    double at(int i);

    void set_indexfinger(const std::tuple<double, double, double> & p);
    void get_indexfinger(double &x, double &y);
    bool is_set_indexfinger();

    void get_palmbase(double &x, double &y);
    void get_palmbase(std::tuple<double, double, double> & p);
    void set_palmbase(const std::tuple<double, double, double> & p);
    void set_palmbase(double x, double y);

    void get_indexbase(double &x, double &y);
    void get_indexbase(std::tuple<double, double, double> & p);
    void set_indexbase(const std::tuple<double, double, double> & p);
    void set_indexbase(double x, double y);

    void set_is_static(bool is_static);
    bool is_static();
};

#endif