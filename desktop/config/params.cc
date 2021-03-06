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

    m_indexbase                         = SmoothCoord("indexbase"                       , 0.9999, 5, 15);
    m_palmbase                          = SmoothCoord("palmbase_left"                   , 0.9999, 5, 15);
    m_primary_cursor                    = SmoothCoord("primary_cursor"                  , 0.9999, 2, 15);
    m_primary_cursor_middlefinger_base  = SmoothCoord("primary_cursor_middlefinger_base", 0.9999, 5, 15);
    m_palmbase_right                    = SmoothCoord("palmbase_right"                  , 0.9999, 5, 15);
    reset();
    init(_load_video);
}


void Parameters::reset() {
    m_indexbase.reset();
    m_palmbase.reset();
    m_primary_cursor.reset();
    m_primary_cursor_middlefinger_base.reset();
    
    is_static_display = false;
    load_video = false;
    m_progress_bar = -1;

    for (int i = 0; i < 4; i ++) m_valid_points[i] = false;
}


void Parameters::init(bool _load_video) {
    load_video = _load_video;
    m_show_depth_txt = false;
}


void Parameters::get_palmbase(double &x_col, double &y_row) {
    m_palmbase.get(x_col, y_row);
}


void Parameters::get_palmbase(std::tuple<double, double, double> & p) {
    m_palmbase.get(p);
}


void Parameters::set_palmbase(const std::tuple<double, double, double> & p) {
    m_palmbase.set(p);
}


void Parameters::set_palmbase_right(const std::tuple<double, double, double> & p) {
    m_palmbase_right.set(p);
}


void Parameters::get_indexbase(std::tuple<double, double, double> & p) {
    m_indexbase.get(p);
}


void Parameters::get_indexbase(double &x_col, double &y_row) {
    m_indexbase.get(x_col, y_row);
}


// void Parameters::set_indexbase(double x_col, double y_row) {
//     m_indexbase.set(x_col, y_row);
// }


void Parameters::set_indexbase(const std::tuple<double, double, double> & p) {
    m_indexbase.set(p);
}

void Parameters::get_palmbase_middle_cv_indices(index_t &x_col, index_t &y_row) {
    double _x1, _y1, _x2, _y2;
    m_palmbase.get(_x1, _y1);
    m_indexbase.get(_x2, _y2);

    // std::cerr << "x1:" << _x1 << "\ty1:" << _y1 << "\tx2:" << _x2 << "\ty2:" << _y2 << "\n";

    x_col = _x1 * m_frame_width;
    y_row = ((_y1 + _y2)/2) * m_frame_height;
}


void Parameters::set_is_static(bool s) {
    is_static_display = s;
}


bool Parameters::is_static() {
    return is_static_display;
}


// the const makes the promise that the func will not change state
int Parameters::total_hands_detected() const {
    return m_total_hands;
}


void Parameters::set_total_hands(int hands) {
    m_total_hands = hands;
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
    m_primary_cursor.get(x, y);
}


void Parameters::set_primary_cursor(const std::tuple<double, double, double> & p) {
    m_primary_cursor.set(p);
}


void Parameters::set_primary_cursor(double x, double y) {
    m_primary_cursor.set(x, y);
}


void Parameters::get_primary_cursor_cv_indices(index_t & x_col, index_t & y_row) {
    get_cv_indices(m_primary_cursor, x_col, y_row);
}


bool Parameters::is_set_primary_cursor() {
    return m_primary_cursor.is_set();
}

cv::Point Parameters::cursor_cvpoint() {
    int x, y;
    get_cv_indices(m_primary_cursor, x, y);
    
    return cv::Point(x, y);
}


void Parameters::get_leftpalm_base_cv_indices(index_t & leftpalm_x_col, index_t & leftpalm_y_row) {
    get_cv_indices(m_palmbase, leftpalm_x_col, leftpalm_y_row);
}


bool Parameters::is_set_leftpalm_base() {
    return m_palmbase.is_set();
}


void Parameters::get_rightpalm_base_cv_indices(index_t &rigthpalm_x_col, index_t & rightpalm_y_row) {
    get_cv_indices(m_palmbase_right, rigthpalm_x_col, rightpalm_y_row);
}


bool Parameters::is_set_rightpalm_base() {
    return m_palmbase_right.is_set();
}


// TODO clean if not used
float Parameters::get_depth(int x_col, int y_row) {
    return m_camera->get_depth(x_col, y_row);
}


void Parameters::set_primary_cursor_middlefinger_base(const std::tuple<double, double, double> & p) {
    m_primary_cursor_middlefinger_base.set(p);
}


void Parameters::get_primary_cursor_middlefinger_base_cv_indices(index_t & x_col, index_t & y_row) {
    get_cv_indices(m_primary_cursor_middlefinger_base, x_col, y_row);
}


void Parameters::get_cv_indices(
    SmoothCoord & point, 
    index_t & x_col, index_t & y_row) {
    
    double _x, _y;
    point.get(_x, _y);
    x_col = (int)(_x * m_frame_width);
    y_row = (int)(_y * m_frame_height);
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


int Parameters::primary_cursor_size() {
    if (!m_valid_points[handedness::RIGHT]) {
        std::cout <<"WARNING: config/params.cc primary_cursor_size() m_valid_points[handedness::RIGHT] false\n";
        return 0;
    }

    return m_hand_size_scale[handedness::RIGHT];
}


int Parameters::primary_cursor_color_size() {
    if (!m_valid_points[handedness::RIGHT]) {
        // std::cout <<"WARNING: config/params.cc primary_cursor_size() m_valid_points[handedness::RIGHT] false\n";
        return 0;
    }

    return m_hand_color_scale[handedness::RIGHT];
}


int palm_ids [] = {0, 1, 5, 9, 13, 17};


std::pair<double, double> Parameters::palm_width() const {
    if (!m_valid_points[handedness::LEFT]) {
        // std::cout <<"WARNING: config/params.cc palm_width() m_valid_points[handedness::LEFT] false\n";
        return {0, 0};
    }

    double _x  = m_points[handedness::LEFT][0].x();
    double mnm = _x;
    double mxm = _x;

    for (int i = 1; i < 6; i ++) {
        _x = m_points[handedness::LEFT][palm_ids[i]].x();
        
        if (mnm > _x) {
            mnm = _x;
        }

        if (mxm < _x) {
            mxm = _x;
        }
    }

    return std::make_pair(
        mnm*m_frame_width, 
        mxm*m_frame_width);
}


std::pair<double, double> Parameters::palm_height() const {
    if (!m_valid_points[handedness::LEFT]) {
        std::cout <<"WARNING: config/params.cc palm_height() m_valid_points[handedness::LEFT] false\n";
        return {0, 0};
    }

    double _y  = m_points[handedness::LEFT][0].y();
    double mnm = _y;
    double mxm = _y;

    for (int i = 1; i < 6; i ++) {
        _y = m_points[handedness::LEFT][palm_ids[i]].y();
        
        if (mnm > _y) {
            mnm = _y;
        }

        if (mxm < _y) {
            mxm = _y;
        }
    }

    
    return std::make_pair(
        mnm*m_frame_height, 
        mxm*m_frame_height);
}


cv::Point Parameters::thumb_tip() {
    if (!m_valid_points[handedness::LEFT]) {
        std::cout <<"WARNING: config/params.cc thumb_tip() m_valid_points[handedness::LEFT] false\n";
        return cv::Point(0, 0);
    }

    return cv::Point(
        m_points[handedness::LEFT][4].x() * m_frame_width,
        m_points[handedness::LEFT][4].y() * m_frame_height
    );
}



cv::Point Parameters::index_tip() {
    if (!m_valid_points[handedness::LEFT]) {
        std::cout <<"WARNING: config/params.cc thumb_base() m_valid_points[handedness::LEFT] false\n";
        return cv::Point(0, 0);
    }

    return cv::Point(
        m_points[handedness::LEFT][8].x() * m_frame_width,
        m_points[handedness::LEFT][8].y() * m_frame_height
    );
}


cv::Point Parameters::thumb_base() {
    if (!m_valid_points[handedness::LEFT]) {
        std::cout <<"WARNING: config/params.cc thumb_base() m_valid_points[handedness::LEFT] false\n";
        return cv::Point(0, 0);
    }

    return cv::Point(
        m_points[handedness::LEFT][2].x() * m_frame_width,
        m_points[handedness::LEFT][2].y() * m_frame_height
    );
}


std::vector<std::vector<std::tuple<double, double, double>>> Parameters::get_points() {
    std::vector<std::vector<std::tuple<double, double, double>>> ret (2);
    for (int i = 0; i < 2; i ++) {
        ret[i] = std::vector<std::tuple<double, double, double>> (21);
        for (int j = 0; j < 21; j ++) {
            ret[i][j] = m_points[i][j].get();
        }
    }

    return ret;
}


std::string Parameters::lefthand_landmarks_str() {
    if (!(m_hand_mask & (1<<0))) return ""; // left palm (0) not detected

    return _get_landmarks(0);
}


std::string Parameters::righthand_landmarks_str() {
    if (!(m_hand_mask & (1<<1))) return ""; // left palm (0) not detected

    return _get_landmarks(1);
}


std::string Parameters:: _get_landmarks(int palm_id) {
    std::string ret = "\"";

    const std::vector<SmoothCoord> & p = m_points[0];

    for (int i = 0; i < 21; i ++) {
        if (i) ret = ret + ",";
        ret = ret + "(" + std::to_string(p[i].x()) + "," + std::to_string(p[i].y()) + ")";
    }

    ret = ret + "\"";

    return ret;
}