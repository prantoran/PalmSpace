// #include "desktop/initiators/initiators.h"
#include "config.h"
#include <iostream>
#include <vector>

ExtraParameters::~ExtraParameters() {
    std::cout << "extraparameters killed\n";
}


ExtraParameters::ExtraParameters() {
    extra_params = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    psize = extra_params.size();

    reset();
}

ExtraParameters::ExtraParameters(int _frame_width, int _frame_height, bool _load_video, Camera * _camera) {
    
    m_frame_height = _frame_height;
    m_frame_width = _frame_width;

    if (_camera == nullptr) {
        std::cout << "ExtraParameters _camera not found\n";
        // TODO create error package and throw custom error
        return;
    }

    m_camera = _camera;

    extra_params = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    psize = extra_params.size();

    reset();
    init(_load_video);
}


void ExtraParameters::reset() {
    for (int i = 0; i < psize; i ++) {
        extra_params[i] = -1;
    }

    m_indexbase = std::make_tuple(-1, -1, -1);
    
    is_static_display = false;
    load_video = false;
}


void ExtraParameters::init(bool _load_video) {
    load_video = _load_video;
}

void ExtraParameters::set(int i, double v) {
    extra_params[i] = v;
}


void ExtraParameters::set(const std::vector<double> & p) {
    for (int i = 0; i < psize; i ++) {
        extra_params[i] = p[i];
    }
}


double ExtraParameters::at(int i) {
    return extra_params[i];
}


void ExtraParameters::set_palmbase(const std::tuple<double, double, double> & p) {
    m_palmbase = p;
}


void ExtraParameters::set_palmbase(double x, double y) {
    m_palmbase = std::make_tuple(x, y, 0);
}


void ExtraParameters::get_palmbase(double &x, double &y) {
    x = std::get<0>(m_palmbase);
    y = std::get<1>(m_palmbase);

    // for adjusting topleft pos of grid, since pointing 
    // to top-left grids with other palm's index finger is hard
    x += 0.11;
    y += 0.16;
}


void ExtraParameters::get_palmbase(std::tuple<double, double, double> & p) {
    double x = std::get<0>(m_palmbase);
    double y = std::get<1>(m_palmbase);

    // for adjusting topleft pos of grid, since pointing 
    // to top-left grids with other palm's index finger is hard
    x += 0.11;
    y += 0.16;

    p = std::make_tuple(x, y, 0);
}


void ExtraParameters::set_indexbase(const std::tuple<double, double, double> & p) {
    m_indexbase = p;
}


void ExtraParameters::set_indexbase(double x, double y) {
    m_indexbase = std::make_tuple(x, y, 0);
}


void ExtraParameters::get_indexbase(double &x, double &y) {
    x = std::get<0>(m_indexbase);
    y = std::get<1>(m_indexbase);
}


void ExtraParameters::get_indexbase_cv_indices(index_t & rowi, index_t & colj) {
    rowi = (int)(std::get<0>(m_indexbase) * m_frame_width);
    colj = (int)(std::get<1>(m_indexbase) * m_frame_height);
}


void ExtraParameters::get_indexbase(std::tuple<double, double, double> & p) {

    double x = std::get<0>(m_indexbase);
    double y = std::get<1>(m_indexbase);

    // for adjusting topleft pos of grid, since pointing 
    // to top-left grids with other palm's index finger is hard
    // x += 0.03;
    // y += 0.09;

    p = std::make_tuple(x, y, 0);
}


void ExtraParameters::get_palmbase_middle_cv_indices(index_t &x_col, index_t &y_row) {
    double _x = std::get<0>(m_palmbase);
    double _y = (std::get<1>(m_palmbase) + std::get<1>(m_indexbase)) / 2;
    x_col = _x * m_frame_width;
    y_row = _y * m_frame_height;
}


void ExtraParameters::set_is_static(bool s) {
    is_static_display = s;
}


bool ExtraParameters::is_static() {
    return is_static_display;
}


// the const makes the promise that the func will not change state
int ExtraParameters::total_hands_detected() const {
    return total_hands;
}


void ExtraParameters::set_total_hands(int hands) {
    total_hands = hands;
}


void ExtraParameters::get_selected_cell(int &row_i, int &col_j) {
    row_i = std::get<0>(selected_cell);
    col_j = std::get<1>(selected_cell);
}

void ExtraParameters::set_selected_cell(index_t row_i, index_t col_j) {
    selected_cell = std::make_tuple(row_i, col_j);
}


void ExtraParameters::set_depth_map(cv::Mat * _mat) {
    depth_mat = _mat;
}


// TODO remove if not needed
void ExtraParameters::get_depth_at(const std::vector<int> & rows, const std::vector<int> & cols, double & value) {
    // expect rows and cols to contain pixel locations in the range [0, W] and [0, H], NOT ratios in [0, 1]
    // depth_math in BGR?
    
    value = 0;
    
    int n = rows.size();
    // std::cerr << "n:" << n << "\n";

    for (int i = 0; i < n; i ++) {
        if (rows[i] < 10 || rows[i] > 620 || cols[i] < 10 || cols[i] > 440) {
            return;
        }
    }

    
    // std::vector<uchar> medians (9*n);
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

    // std::cout << "mi:" << mi << "\n";

    std::sort(medians, medians+mi);
    

    // for (int i = 0; i < mi; i ++) std::cerr << medians[i] << " ";
    // std::cerr << "\n";

    value = (double) medians[mi/2];
    // cv::Mat roi(*depth_mat, cv::Rect(row-5, col-5, 10, 10));
    // std::cerr << "pass roi init\n";
    // // std::cerr << "roi.x:" << roi.x << " roi.y:" << roi.y << " roi.width:" << roi.width << " roi.height:" << roi.height << "\n";
    // cv::Scalar m = cv::mean(roi);

    // value = (int)m[0]; // depth_mat is 1 channel
}


void ExtraParameters::get_primary_cursor(double & x, double & y) {
    x = std::get<0>(m_primary_cursor);
    y = std::get<1>(m_primary_cursor);
}


void ExtraParameters::set_primary_cursor(const std::tuple<double, double, double> & p) {
    m_primary_cursor = p;
}


void ExtraParameters::set_primary_cursor(double x, double y) {
    m_primary_cursor = std::make_tuple(x, y, 0);
}


void ExtraParameters::get_primary_cursor_cv_indices(index_t & x_col, index_t & y_row) {
    get_cv_indices(m_primary_cursor, x_col, y_row);
}


bool ExtraParameters::is_set_primary_cursor() {
    return std::get<0>(m_primary_cursor) > 0.01;
}


// TODO clean if not used
float ExtraParameters::get_depth(int x_col, int y_row) {
    return m_camera->get_depth(x_col, y_row);
}


void ExtraParameters::set_primary_cursor_middlefinger_base(const std::tuple<double, double, double> & p) {
    m_primary_cursor_middlefinger_base = p;
}


void ExtraParameters::get_primary_cursor_middlefinger_base_cv_indices(index_t & x_col, index_t & y_row) {
    get_cv_indices(m_primary_cursor_middlefinger_base, x_col, y_row);
}


void ExtraParameters::get_cv_indices(
    const std::tuple<double, double, double> & point, 
    index_t & x_col, index_t & y_row) {

    x_col = (int)(std::get<0>(point) * m_frame_width);
    y_row = (int)(std::get<1>(point) * m_frame_height);
}