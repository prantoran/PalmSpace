#include "grid.h"

#include <iostream>

Grid::Grid() {
    m_gap = 1;
    m_dynamic_dimensions = false;
}


void Grid::reset_minimum_dimensions() {
    m_width_min = 0;
    m_height_min = 0;
}


void Grid::reset_dimensions() {
    m_width = 0;
    m_height = 0;
}


cv::Point Grid::get_bottom_right() const {
    return cv::Point(m_x_cols[0]+m_width, m_y_rows[0]+m_height);
}


cv::Point Grid::get_top_left() const {
    return cv::Point(m_x_cols[0], m_y_rows[0]);
}


cv::Point Grid::get_top_right() const {
    return cv::Point(m_x_cols[0]+m_width, m_y_rows[0]);
}


cv::Point Grid::get_bottom_left() const {
    return cv::Point(m_x_cols[0], m_y_rows[0]+m_height);
}


cv::Rect Grid::get_bound_rect() const {
    return cv::Rect(get_top_left(), get_bottom_right());
}


void Grid::reset() {
    int len = (sizeof(m_x_cols)/sizeof(*m_x_cols));
    for (int i = 0; i < len; i ++) {
        m_x_cols[i] = 0;
        m_y_rows[i] = 0;
    }
}


int Grid::arg_x(int pointer_x) const {
    for (int j = 1;j <= m_divisions; j ++) {
        if (pointer_x >= m_x_cols[j] && pointer_x <= m_x_cols[j] + m_dx_col) {
            return j;
        }     
    }

    return -1;
}


int Grid::arg_y(int pointer_y) const {
    for (int i = 1;i <= m_divisions; i ++) {
        if (pointer_y >= m_y_rows[i] && pointer_y <= m_y_rows[i] + m_dy_row) {
        return i;
        }
    }

    return -1;
}


void Grid::align(double topleft_x, double topleft_y) {
    // topleft coords are actual coords in the range [0, width-1] and [0, height-1] respectively
    // setup tile coords 
    m_width = m_width_min;
    m_height = m_height_min;

    m_dx_col = (m_width - (m_divisions+1)*m_gap)/m_divisions;
    m_dy_row = (m_height - (m_divisions+1)*m_gap)/m_divisions;

    m_x_cols[0] = topleft_x;
    m_x_cols[1] = m_x_cols[0]+m_gap;
    for (int i = 2; i <= m_divisions; i ++) {
      m_x_cols[i] = m_x_cols[i-1]+m_dx_col+m_gap;
    }

    m_y_rows[0] = topleft_y;
    m_y_rows[1] = m_y_rows[0]+m_gap;
    for (int i = 2; i <= m_divisions; i ++) {
      m_y_rows[i] = m_y_rows[i-1]+m_dy_row+m_gap;
    }
}


cv::Rect Grid::get_cell(int i, int j) const {
    // grid indices start from 1 and includes n
    // index 0 is leftmost/topmost point
    return cv::Rect(
        cv::Point(m_x_cols[i], m_y_rows[j]), 
        cv::Point(m_x_cols[i]+m_dx_col, m_y_rows[j]+m_dy_row)
    );
}


void Grid::get_center_cv(int & _col_width, int & _row_height) {
    _col_width  = m_x_cols[0] +  m_width/2;
    _row_height = m_y_rows[0] + m_height/2;
}


bool Grid::is_inside_cv(int _col_x, int _row_y) const {
    // std::cout << "is_inside_cv() _col_x:" << _col_x << "\t_row_y:" << _row_y << 
    //     "\tm_x_cols[0]:" << m_x_cols[0] << "\t(m_x_cols[0] + m_width + 10):"  << (m_x_cols[0] + m_width + 10)  << 
    //     "\tm_y_rows[0]:" << m_y_rows[0] << "\t(m_y_rows[0] + m_height + 10):" << (m_y_rows[0] + m_height + 10) << "\n";
    return _col_x >= m_x_cols[0] && _col_x <= (m_x_cols[0] + m_width + 10) && 
        _row_y >= m_y_rows[0] && _row_y <= (m_y_rows[0] + m_height + 10);
}