#include "coord.h"

SmoothCoord::SmoothCoord() {
    m_momentum = 0;
    init(15, 15);
    reset();
}


SmoothCoord::SmoothCoord(
    std::string _name, 
    double _momentum, 
    type_t _diff_drop_thresh,
    int _max_drop_cnt) {

    m_momentum = _momentum;
    m_name = _name;
    init(_diff_drop_thresh, _max_drop_cnt);
    reset();
}

void SmoothCoord::init(type_t _diff_drop_thresh, int _max_drop_cnt) {
    m_max_drop_cnt = _max_drop_cnt;
    m_diff_drop_thresh = _diff_drop_thresh;
    m_diff_ratio_drop_thresh = 50;
}


void SmoothCoord::reset_velocity() {
    m_dx = -1, m_dy = -1, m_dz = -1;
}


void SmoothCoord::reset() {
    m_cur = std::make_tuple(-1, -1, -1);
    m_drop_cnt = 0;
    reset_velocity();
}


void SmoothCoord::update_velocity() {
    _update_v(m_dx, m_dx_p);
    _update_v(m_dy, m_dy_p);
    _update_v(m_dz, m_dz_p);
}


void SmoothCoord::_update_v(type_t & _d, type_t & _d_p) {
    _d_p = _d;
    if (_d_p == -1) return;
    // std::cout << "using momentum\n";
    _d = (1-m_momentum)*_d + m_momentum*_d_p;  
}


bool SmoothCoord::_check_thresh(type_t _d, type_t _d_p) const {
    // if (_d_p && ((_d/_d_p) > m_diff_ratio_drop_thresh || (_d/_d_p) < -m_diff_ratio_drop_thresh)) {
    //     return true;
    // }

    return _d > m_diff_drop_thresh || _d < -m_diff_drop_thresh;
}


void SmoothCoord::set(const std::tuple<type_t, type_t, type_t> & _new_point) {
    m_prev = m_cur; 
    if(std::get<0>(_new_point) < 0) {
        // std::cout << "m_name:" << m_name << "\n";
        // if (m_name == "H_0_L_8") {
        //     std::cout << m_name << "WARN config/coord.cc set(): resetting NOT expected behaviour\n";
        // }
        m_cur = _new_point;
    } else if (std::get<0>(m_prev) < 0.001) {
        m_cur = _new_point;
    } else {

        // _new_point >= {0, 0, 0}

        m_dx = std::get<0>(_new_point) - std::get<0>(m_prev);
        m_dy = std::get<1>(_new_point) - std::get<1>(m_prev);
        m_dz = std::get<2>(_new_point) - std::get<2>(m_prev);

        // if (m_name == "H_0L_8") {
        //     std::cout << "m_dx:" << m_dx << "\tm_dy:" << m_dy << "\td_dz:" << m_dz << "\n";
        // }

        if (_check_thresh(m_dx, m_dx_p) || _check_thresh(m_dy, m_dy_p) || _check_thresh(m_dz, m_dz_p)) {

            m_drop_cnt ++;
            if (m_drop_cnt < m_max_drop_cnt) {
                return;
            }

            reset_velocity();
        }

        m_drop_cnt = 0;
        
        update_velocity();


        update_pos();
    }
}


void SmoothCoord::update_pos() {
    type_t _x = std::get<0>(m_cur);
    type_t _y = std::get<1>(m_cur);
    type_t _z = std::get<2>(m_cur);

    _update_pos(_x, std::get<0>(m_prev), m_dx);
    _update_pos(_y, std::get<1>(m_prev), m_dy);
    _update_pos(_z, std::get<2>(m_prev), m_dz);

    m_cur = std::make_tuple(_x, _y, _z);
}


void SmoothCoord::_update_pos(type_t & _pos, type_t _pos_p, type_t _velocity) {
    if (_pos_p == -1) {
        return;
    }

    // if (m_name == "H_0L_8") {
        // std::cout << "using velocity to update pos v:" << _velocity << "\t_pos_p:" << _pos_p << "\n";
    // }

    _pos = _pos_p + 0.5*_velocity;
}


void SmoothCoord::set(const type_t & _x_col, const type_t & _y_row) {
    set(std::make_tuple(_x_col, _y_row, 0.0));
}


void SmoothCoord::get(type_t & x_col, type_t & y_row) {
    if (is_set()) {
        x_col = std::get<0>(m_cur);
        y_row = std::get<1>(m_cur);
    } else {
        // std::cout << m_name << " value not set\n";
        x_col = -1;
        y_row = -1;
    }
}


void SmoothCoord::get(std::tuple<type_t, type_t, type_t> & _point) {
    _point = m_cur;
}


std::tuple<double, double, double> SmoothCoord::get() const {
    return m_cur;
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


void SmoothCoord::set_momentum(double _momentum) {
    m_momentum = _momentum;
}


bool SmoothCoord::is_set() {
    return std::get<0>(m_cur) > 0.01;
}


double SmoothCoord::x() const {
    return std::get<0>(m_cur);
}


double SmoothCoord::y() const {
    return std::get<1>(m_cur);
}


double SmoothCoord::z() const {
    return std::get<2>(m_cur);
}