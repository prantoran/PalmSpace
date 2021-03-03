#ifndef COORD_H
#define COORD_H

#include <tuple>
#include <iostream>


class SmoothCoord {

  using type_t = double;
  std::tuple<type_t, type_t, type_t> m_cur, m_prev;
  double m_momentum;
  std::string m_name;

  type_t m_dx, m_dy, m_dz;
  type_t m_dx_p, m_dy_p, m_dz_p;

  int m_drop_cnt, m_max_drop_cnt;
  type_t m_diff_drop_thresh;
  type_t m_diff_ratio_drop_thresh;

  bool m_reset;

  void reset_velocity();
  void update_velocity();
  void _update_v(type_t & _d, type_t & _d_p);
  void update_pos();
  void _update_pos(type_t & _pos, type_t _pos_p, type_t _velocity);
  void init(type_t _diff_drop_thresh, int _max_drop_cnt);
  bool _check_thresh(type_t _d, type_t _d_p) const;

  public:
  SmoothCoord();
  SmoothCoord(
    std::string _name, 
    double _momentum, 
    type_t _diff_drop_thresh,
    int _max_drop_cnt);
  void reset();
  void set(const std::tuple<type_t, type_t, type_t> & _new_point);
  void set(const type_t & _x_col, const type_t & _y_row);
  void get(std::tuple<type_t, type_t, type_t> & _point);
  void get(type_t & x_col, type_t & y_row);
  void get(type_t & x_col, type_t & y_row, type_t &z_depth);
  std::tuple<double, double, double> get() const;
  void set_momentum(double _momentum);
  bool is_set();
  double x() const;
  double y() const;
  double z() const;
};


#endif