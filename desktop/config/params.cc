// #include "desktop/initiators/initiators.h"
#include "config.h"
#include <iostream>
#include <vector>

Parameters::~Parameters() {
    std::cout << "Parameters killed\n";
}


Parameters::Parameters() {
    reset();
}

Parameters::Parameters(int _frame_width, int _frame_height, bool _load_video, Camera * _camera) {
    
    m_frame_height = _frame_height;
    m_frame_width = _frame_width;

    if (_camera == nullptr) {
        std::cout << "Parameters _camera not found\n";
        // TODO create error package and throw custom error
        return;
    }

    m_camera = _camera;

    reset();
    init(_load_video);
}


void Parameters::reset() {
    m_indexbase = std::make_tuple(-1, -1, -1);
    
    is_static_display = false;
    load_video = false;

    m_progress_bar = -1;
}


void Parameters::init(bool _load_video) {
    load_video = _load_video;
    m_show_depth_txt = false;
}


void Parameters::set_palmbase(const std::tuple<double, double, double> & p) {
    m_palmbase = p;
}


void Parameters::set_palmbase(double x, double y) {
    m_palmbase = std::make_tuple(x, y, 0);
}


void Parameters::get_palmbase(double &x, double &y) {
    x = std::get<0>(m_palmbase);
    y = std::get<1>(m_palmbase);

    // for adjusting topleft pos of grid, since pointing 
    // to top-left grids with other palm's index finger is hard
    x += 0.11;
    y += 0.16;
}


void Parameters::get_palmbase(std::tuple<double, double, double> & p) {
    double x = std::get<0>(m_palmbase);
    double y = std::get<1>(m_palmbase);

    // for adjusting topleft pos of grid, since pointing 
    // to top-left grids with other palm's index finger is hard
    x += 0.11;
    y += 0.16;

    p = std::make_tuple(x, y, 0);
}


void Parameters::set_indexbase(const std::tuple<double, double, double> & p) {
    m_indexbase = p;
}


void Parameters::set_indexbase(double x, double y) {
    m_indexbase = std::make_tuple(x, y, 0);
}


void Parameters::get_indexbase(double &x, double &y) {
    x = std::get<0>(m_indexbase);
    y = std::get<1>(m_indexbase);
}


void Parameters::get_indexbase_cv_indices(index_t & rowi, index_t & colj) {
    rowi = (int)(std::get<0>(m_indexbase) * m_frame_width);
    colj = (int)(std::get<1>(m_indexbase) * m_frame_height);
}


void Parameters::get_indexbase(std::tuple<double, double, double> & p) {

    double x = std::get<0>(m_indexbase);
    double y = std::get<1>(m_indexbase);

    p = std::make_tuple(x, y, 0);
}


void Parameters::get_palmbase_middle_cv_indices(index_t &x_col, index_t &y_row) {
    double _x = std::get<0>(m_palmbase);
    double _y = (std::get<1>(m_palmbase) + std::get<1>(m_indexbase)) / 2;
    x_col = _x * m_frame_width;
    y_row = _y * m_frame_height;
}


void Parameters::set_is_static(bool s) {
    is_static_display = s;
}


bool Parameters::is_static() {
    return is_static_display;
}


// the const makes the promise that the func will not change state
int Parameters::total_hands_detected() const {
    return total_hands;
}


void Parameters::set_total_hands(int hands) {
    total_hands = hands;
}


void Parameters::get_selected_cell(int &row_i, int &col_j) {
    row_i = std::get<0>(selected_cell);
    col_j = std::get<1>(selected_cell);
}

void Parameters::set_selected_cell(index_t row_i, index_t col_j) {
    selected_cell = std::make_tuple(row_i, col_j);
}


void Parameters::set_depth_map(cv::Mat * _mat) {
    depth_mat = _mat;
}


// TODO remove if not needed
void Parameters::get_depth_at(const std::vector<int> & rows, const std::vector<int> & cols, double & value) {
    // expect rows and cols to contain pixel locations in the range [0, W] and [0, H], NOT ratios in [0, 1]
    // depth_math in BGR?
    
    value = 0;
    
    int n = rows.size();

    for (int i = 0; i < n; i ++) {
        if (rows[i] < 10 || rows[i] > 620 || cols[i] < 10 || cols[i] > 440) {
            return;
        }
    }

    int mi = 0;
    for (int k = 0; k < n; k ++) {
        int row = rows[k];
        int col = cols[k];

        m_row = row;
        m_col = col;
        float curd;

        for (int i = row-1; i < row+2; i ++) {
            for (int j = col -1; j < col + 2; j ++) {
                // if (!(int)(*depth_mat).at<cv::Vec3b>(i, j)[2]) continue;
                // u = (*depth_mat).at<cv::Vec3b>(i, j)[2] - (*depth_mat).at<cv::Vec3b>(i, j)[1] - (*depth_mat).at<cv::Vec3b>(i, j)[0]/2;
                curd = m_camera->get_depth(i, j);
                if (curd < 1e-6) continue;
                medians[mi++] = curd;
            }
        }
    }

    std::sort(medians, medians+mi);

    value = (double) medians[mi/2];
}


void Parameters::get_primary_cursor(double & x, double & y) {
    x = std::get<0>(m_primary_cursor);
    y = std::get<1>(m_primary_cursor);
}


void Parameters::set_primary_cursor(const std::tuple<double, double, double> & p) {
    m_primary_cursor = p;
}


void Parameters::set_primary_cursor(double x, double y) {
    m_primary_cursor = std::make_tuple(x, y, 0);
}


void Parameters::get_primary_cursor_cv_indices(index_t & x_col, index_t & y_row) {
    get_cv_indices(m_primary_cursor, x_col, y_row);
}


bool Parameters::is_set_primary_cursor() {
    return std::get<0>(m_primary_cursor) > 0.01;
}


// TODO clean if not used
float Parameters::get_depth(int x_col, int y_row) {
    return m_camera->get_depth(x_col, y_row);
}


void Parameters::set_primary_cursor_middlefinger_base(const std::tuple<double, double, double> & p) {
    m_primary_cursor_middlefinger_base = p;
}


void Parameters::get_primary_cursor_middlefinger_base_cv_indices(index_t & x_col, index_t & y_row) {
    get_cv_indices(m_primary_cursor_middlefinger_base, x_col, y_row);
}


void Parameters::get_cv_indices(
    const std::tuple<double, double, double> & point, 
    index_t & x_col, index_t & y_row) {

    x_col = (int)(std::get<0>(point) * m_frame_width);
    y_row = (int)(std::get<1>(point) * m_frame_height);
}


void Parameters::get_raw_dimensions(double & _width, double _height) {
    _width = m_raw_dimensions.first;
    _height = m_raw_dimensions.second;
}


void Parameters::set_raw_dimensions(double _width, double _height) {
    m_raw_dimensions.first = _width;
    m_raw_dimensions.second = _height;
}


void Parameters::set_progress_bar(double _progress) {
    m_progress_bar = _progress;
}


void Parameters::get_progress_bar(double & _progress) {
    _progress = m_progress_bar;
}


void Parameters::set_other_index() { // TODO need more inspection
}


void Parameters::get_other_index_z_value(double _z) {
    _z = std::get<2>(m_other_index);
}