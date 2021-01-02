#include "config.h"

SmoothCoord::SmoothCoord() {
    m_decay = 1;
    reset();
}


SmoothCoord::SmoothCoord(std::string _name, double _decay) {
    m_decay = _decay;
    m_name = _name;
    reset();
}


void SmoothCoord::reset() {
    m_cur = std::make_tuple(-1, -1, -1);
}


void SmoothCoord::set(const std::tuple<type_t, type_t, type_t> & _new_point) {
    m_prev = m_cur; 
    if (std::get<0>(m_prev) < 0.01) {
        m_cur = _new_point;
    } else if(std::get<0>(_new_point) < 0) {
        std::cout << m_name << ": resetting in set func() NOT expected behaviour\n";
        m_cur = _new_point;
    } else {
        m_cur =  std::make_tuple(
            m_decay*std::get<0>(_new_point) + (1-m_decay)*std::get<0>(m_prev),
            m_decay*std::get<1>(_new_point) + (1-m_decay)*std::get<1>(m_prev),
            m_decay*std::get<2>(_new_point) + (1-m_decay)*std::get<2>(m_prev)
        );
    }
}


void SmoothCoord::set(const type_t & _x_col, const type_t & _y_row) {
    set(std::make_tuple(_x_col, _y_row, 0));
}


void SmoothCoord::get(type_t & x_col, type_t & y_row) {
    if (is_set()) {
        x_col = std::get<0>(m_cur);
        y_row = std::get<1>(m_cur);
    } else {
        std::cout << m_name << " value not set\n";
        x_col = -1;
        y_row = -1;
    }
}


void SmoothCoord::get(std::tuple<type_t, type_t, type_t> & _point) {
    _point = m_cur;
}


void SmoothCoord::get(type_t & x_col, type_t & y_row, type_t &z_depth) {
    if (is_set()) {
        x_col = std::get<0>(m_cur);
        y_row = std::get<1>(m_cur);
        z_depth = std::get<2>(m_cur);
    } else {
        std::cout << m_name << " value not set\n";
        x_col = -1;
        y_row = -1;
        z_depth = -1;
    }
}


void SmoothCoord::set_decay(double _decay) {
    m_decay = _decay;
}


bool SmoothCoord::is_set() {
    return std::get<0>(m_cur) > 0.01;
}