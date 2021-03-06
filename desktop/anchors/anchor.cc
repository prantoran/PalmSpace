#include "anchor.h"


Anchor::Anchor() {
    m_cur_time_id = 0;
    m_past_selections_size = 15;
    m_past_selections = std::vector<std::pair<int, int>> (m_past_selections_size);
    reset_last_visited_cell_time();
}


Anchor::~Anchor() {
}


void Anchor::setConfig(int _width, int _height) {
    width = _width;
    height = _height;
    progress_maxwidth = width/8;
    progress_maxheight = height/28;
}

std::tuple<int, int> Anchor::selectedIndexes() {
    return std::make_tuple(m_selected_i, m_selected_j);
}

void Anchor::markSelected() {
    m_marked_i = m_selected_i;
    m_marked_j = m_selected_j;

    if (m_marked_i > 0 && m_marked_j > 0) {
        msg_marked = "Marked: " + std::to_string(m_grid.m_divisions*(m_marked_j-1) + m_marked_i);
    } else {
        msg_marked = "Marked -";
    }
}

void Anchor::setDivisions(int _divisions) {
    m_grid.m_divisions = _divisions;
    m_grid_out.m_divisions = _divisions;
}

int Anchor::getDivisions() {
    return m_grid.m_divisions;
}


void Anchor::drawProgressBar(cv::Mat & _image, Parameters & params) {
    params.get_progress_bar(m_progress_bar);
    // modifies _image with a progress bar with _progress
    if (m_progress_bar >= 0) {

        double pwidth = m_progress_bar * (progress_maxwidth);

        // double npwidth = progress_maxwidth - pwidth;
        cv::rectangle(
            _image,
            cv::Point(10, height-10-progress_maxheight),
            cv::Point(10+pwidth, height-10),
            color_green, //green
            cv::FILLED,
            8,
            0
        );

        cv::rectangle(
            _image,
            cv::Point(10+pwidth, height-10-progress_maxheight),
            cv::Point(10+progress_maxwidth, height-10),
            COLORS_grey, // blue
            cv::FILLED,
            8,
            0
        );

        cv::rectangle(
            _image,
            cv::Point(10, height-10-progress_maxheight),
            cv::Point(10+progress_maxwidth, height-10),
            COLORS_grey, // blue
            2,
            8,
            0
        );
    }
}


void Anchor::setupSelection(
    int cursor_x, int cursor_y, 
    int & selected_row_i, int & selected_col_j) {
    // checks whether index_pointer is within a til in the grid,
    // if yes then setup selected i,j and message
    
    m_selected_i_prv = selected_row_i;
    m_selected_j_prv = selected_col_j;

    if (m_selection_locked) {
        // selection locked because trigger is pressed
        return; 
    } 
    
    selected_row_i = m_grid.arg_x(cursor_x);
    selected_col_j = m_grid.arg_y(cursor_y);
    

    if (selected_row_i == -1 || selected_col_j == -1) {
        selected_row_i = -1;
        selected_col_j = -1;
    }
    
    if (selected_row_i != -1) {
        if (selected_row_i != m_selected_i_prv || selected_col_j != m_selected_j_prv) {
            update_last_visited_cell_time(selected_row_i, selected_col_j);
            message = "Highlighted: ";
            message += std::to_string((selected_col_j-1)*m_grid.m_divisions + selected_row_i); // selected_[col,row]_[i,j] 1-indexed
            m_visited_cells ++;
        }
    } 

    if (selected_row_i >= 0 && selected_col_j >= 0) {
        m_past_selections[m_cur_time_id] = {selected_row_i, selected_col_j};
    // selected_row_i = m_past_selections[(m_cur_time_id-1+m_past_selections_size)%m_past_selections_size].first;
    // selected_col_j = m_past_selections[(m_cur_time_id-1+m_past_selections_size)%m_past_selections_size].second;
        m_cur_time_id = (m_cur_time_id + 1) % m_past_selections_size;
    }
     
}


void Anchor::drawTextHighlighted(cv::Mat & overlay) {
    if (m_selected_i != -1) {
        cv::putText(overlay, //target image
            message, //text, set in anchors.cc
            cv::Point(5, 40), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            1.0,
            CV_RGB(240, 240, 240), //font color
            2
        );
    }
}


void Anchor::drawTextMarked(cv::Mat & overlay) {
    if (m_marked_i != -1) {
        cv::putText(overlay, //target image
            msg_marked, //text
            cv::Point(5, 80), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            1.0,
            color_green, //font color
            3
        );
    }
}


void Anchor::reset_palmbase() {
    palmbase_x = -1;
    palmbase_y = -1;
    // TODO check whether logical to reset grid dimensions
    m_grid.reset_dimensions();
    m_grid_out.reset_dimensions();
}


void Anchor::reset_palmbase_right() {
    palmbase_right_x = -1;
    palmbase_right_y = -1;
}


void Anchor::reset_indexbase() {
    indexbase_x = -1;
    indexbase_y = -1;
    // TODO check whether logical to reset grid dimensions
    m_grid.reset_dimensions();
    m_grid_out.reset_dimensions();
}

void Anchor::reset_grids() {
    m_grid.reset();
    m_grid_out.reset();
}


void Anchor::setScreenSize(const choices::screensize::types & size) {
    m_screen.size = size;
}

void Anchor::setVisibility(const choices::visibility::types & _visibility) {
    visibility = _visibility;
}


choices::visibility::types Anchor::getVisibility() {
    return visibility;
}


bool Anchor::isVisible(const Parameters & params) {
    switch (visibility) {
        case choices::visibility::FIXED:
            return true;
        case choices::visibility::CONDITIONAL:
            return (params.total_hands_detected() > 0);
        default:
            std::cout << "ERROR anchors/anchors.cc isVisible() invalid choice visibility\n.";
            return false;
    }
}


choices::anchor::types Anchor::type() {
    return m_type;
}


void Anchor::lock_selection() {
    m_selection_locked = true;
}


void Anchor::unlock_selection() {
    m_selection_locked = false;
}


void Anchor::draw_main_grid_layout(cv::Mat & src, const Grid & grid) {
    ui::rounded_rectangle(
        src, 
        grid.get_top_left(), grid.get_bottom_right(), 
        COLORS_floralwhite,
        5, 
        cv::LINE_8,
        3);
}


void Anchor::draw_cells(cv::Mat & src, const Grid & grid) {
        
    for (int i = 1; i <= grid.m_divisions; i ++ ) {
        for (int j = 1; j <= grid.m_divisions; j ++) {
            color_cur = color_red;
            if (i == m_marked_i && j == m_marked_j) {
                color_cur = color_green;
            }

            cv::rectangle(
                src, 
                grid.get_cell(i, j), 
                color_cur,
                -1, 
                cv::LINE_8,
                0);
            
        }
    }

    if (m_selected_i != -1 && m_selected_j != -1) {
        cv::Rect cell = grid.get_cell(m_selected_i, m_selected_j);
        ui::clear_rectangle(
            src,
            cv::Point(cell.x-1, cell.y-1),
            cv::Point(cell.x-1, cell.y+cell.height+1),
            cv::Point(cell.x+cell.width+1, cell.y+cell.height+1),
            cv::Point(cell.x+cell.width+1, cell.y-1),
            color_blue
        );
    }
}


void Anchor::adjust_selection_prior_trigger() {
    m_selected_i = m_past_selections[(m_cur_time_id-2+m_past_selections_size)%m_past_selections_size].first;
    m_selected_j = m_past_selections[(m_cur_time_id-2+m_past_selections_size)%m_past_selections_size].second;
}


void Anchor::reset_selection() {
    m_selected_i_prv = -1, m_selected_j_prv = -1;
    m_selected_i = -1, m_selected_j = -1;
}


void Anchor::reset_marked_cell() {
    m_marked_i = -1, m_marked_j = -1;
}



void Anchor::setup_background(cv::Mat & _background, std::string _imagePath, int _width, int _height) {
    _background = cv::imread(_imagePath, CV_LOAD_IMAGE_UNCHANGED);
    cv::resize(_background, _background, cv::Size(_width, _height));
}


void Anchor::update_last_visited_cell_time(int _row_i, int _col_j) {
    m_last_time_visited[_row_i][_col_j] = std::chrono::steady_clock::now();
}


void Anchor::reset_last_visited_cell_time() {
    const auto curtime = std::chrono::steady_clock::now();
    
    for (int i = 0; i < 11; i ++) {
        for (int j = 0; j < 11; j ++) {
            m_last_time_visited[i][j] = curtime;
        }
    }
}


std::chrono::time_point<std::chrono::steady_clock> Anchor::get_last_visited_cell_time(int _row_i, int _col_j) {
    // _row_i, _col_j 0-indexed, from trial target
    return m_last_time_visited[_row_i][_col_j];
}