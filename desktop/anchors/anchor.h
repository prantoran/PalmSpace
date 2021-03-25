#ifndef ANCHOR_H 
#define ANCHOR_H


#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "desktop/ui/ui.h"
#include "desktop/config/config.h"
#include "desktop/config/colors.h"
#include "screensize.h"


constexpr double TRANSPARENCY_ALPHA = 0.4;


class Anchor { // interface via abstract class
    choices::visibility::types visibility;
    double m_progress_bar;

    public:
    choices::anchor::types m_type; 
    std::string name;
    int width = 0, height = 0; // TODO ps append m_
    Grid m_grid, m_grid_out;

    double palmbase_x, palmbase_y;
    double palmbase_right_x, palmbase_right_y;
    double indexbase_x, indexbase_y;

    cv::Scalar color_red, color_blue, color_green, color_cur;

    int m_selected_i, m_selected_j, m_selected_i_prv, m_selected_j_prv; 

    int m_marked_i, m_marked_j;
    
    std::string message, msg_marked;

    bool m_static_display; // if true then keep on showing display

    int progress_maxwidth, progress_maxheight; // progress bar
    int pwidth, npwidth;

    ScreenSize m_screen;

    bool m_selection_locked;

    std::vector<std::pair<int, int>> m_past_selections;
    int m_cur_time_id, m_past_selections_size;
    
    int m_visited_cells;

    bool m_absolute;

    choices::inputspace::types m_inputspace_type;
    std::pair<double, double> m_palm_x, m_palm_y;
    double m_max_dim;

    bool m_calculate_done;

    // records the last time the cursor changed from another cell to target cell 
    std::chrono::time_point<std::chrono::steady_clock> m_last_time_visited[11][11];

    
    Anchor();
    virtual ~Anchor();

    void setConfig(int _width, int _height);

    // pure virtual functions
    virtual void calculate(
        const cv::Mat& input,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params) = 0;
    
    virtual void draw(
        const cv::Mat& input, 
        cv::Mat& output,  
        double scale_ratio, 
        int pointer_x, int pointer_y,
        Parameters & params) = 0;

    std::tuple<int, int> selectedIndexes();

    void markSelected();
    void setDivisions(int _divisions);
    int getDivisions();
    void drawProgressBar(cv::Mat & _image, Parameters & params);

    void setupGrid(double enlarged_topleft_x, double enlarged_topleft_y);
    void setupSelection(int cursor_x, int cursor_y, 
        int & selected_row_i, int & selected_col_j);
    
    void drawTextHighlighted(cv::Mat & overlay);
    void drawTextMarked(cv::Mat & overlay);
    
    void reset_palmbase();
    void reset_palmbase_right();
    void reset_indexbase();
    void reset_grids();

    void setScreenSize(const choices::screensize::types & size);

    void setVisibility(const choices::visibility::types & _visibility);
    choices::visibility::types getVisibility();
    bool isVisible(const Parameters & params);

    choices::anchor::types type();

    void lock_selection();
    void unlock_selection();
    void draw_main_grid_layout(cv::Mat & src, const Grid & grid);
    void draw_cells(cv::Mat & src, const Grid & grid);

    void adjust_selection_prior_trigger();

    void reset_selection();
    void reset_marked_cell();
    void setup_background(
        cv::Mat & _background, 
        std::string _imagePath, 
        int _width, 
        int _height);
    
    void reset_last_visited_cell_time();
    void update_last_visited_cell_time(int _row_i, int _col_j);
    std::chrono::time_point<std::chrono::steady_clock> get_last_visited_cell_time(int _row_i, int _col_j);
};


#endif