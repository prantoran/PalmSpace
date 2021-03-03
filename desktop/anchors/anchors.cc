#include "anchors.h"

#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"


Anchor::Anchor() {
    m_cur_time_id = 0;
    m_past_selections_size = 15;
    m_past_selections = std::vector<std::pair<int, int>> (m_past_selections_size);
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

void Anchor::highlightSelected() {
    green_i = m_selected_i;
    green_j = m_selected_j;

    message_selected = "Selected: " + std::to_string(m_grid.m_divisions*(green_j-1) + green_i);
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
    int index_pointer_x, int index_pointer_y, 
    int & selected_row_i, int & selected_col_j) {
    // checks whether index_pointer is within a til in the grid,
    // if yes then setup selected i,j and message
    
    m_selected_i_prv = selected_row_i;
    m_selected_j_prv = selected_col_j;

    if (m_selection_locked) {
        // selection locked becaurse trigger is pressed
        return; 
    } 

    if (index_pointer_x != -1 && index_pointer_y != -1) {

        selected_row_i = m_grid.arg_x(index_pointer_x);
        selected_col_j = m_grid.arg_y(index_pointer_y);

        if (selected_row_i == -1 || selected_col_j == -1) {
            selected_row_i = -1;
            selected_col_j = -1;
        }

        if (selected_row_i != -1) {
            if (selected_row_i != m_selected_i_prv || selected_col_j != m_selected_j_prv) {
                message = "Highlighted: ";
                message += std::to_string((selected_col_j-1)*m_grid.m_divisions + selected_row_i);
            }
        } else {
            selected_row_i = m_selected_i_prv;
            selected_col_j = m_selected_j_prv;
        }

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


void Anchor::drawTextSelected(cv::Mat & overlay) {
    if (green_i != -1) {
        cv::putText(overlay, //target image
            message_selected, //text
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


void Anchor::setScreenSize(const choices::eScreenSize & size) {
    m_screen.size = size;
}

void Anchor::setVisibility(const choices::eVisibility & _visibility) {
    visibility = _visibility;
}


choices::eVisibility Anchor::getVisibility() {
    return visibility;
}


bool Anchor::isVisible(const Parameters & params) {
    switch (visibility) {
        case choices::FIXED:
            return true;
        case choices::CONDITIONAL:
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
            if (i == green_i && j == green_j) {
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



bool Anchor::is_selection_changed() {
    return !((m_selected_i == m_selected_i_prv) && (m_selected_j == m_selected_j_prv));
}


void Anchor::reset_selection_prior_trigger() {
    m_selected_i = m_past_selections[(m_cur_time_id+3)%m_past_selections_size].first;
    m_selected_j = m_past_selections[(m_cur_time_id+3)%m_past_selections_size].second;
}