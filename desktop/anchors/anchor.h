#ifndef ANCHOR_H 
#define ANCHOR_H


#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "desktop/ui/ui.h"
#include "desktop/config/config.h"
#include "desktop/config/colors.h"
#include "screensize.h"


class Anchor { // interface via abstract class
    choices::eVisibility visibility;
    double m_progress_bar;

    public:
    choices::anchor::types m_type; 
    std::string name;
    int width = 0, height = 0; // TODO ps append m_
    Grid m_grid, m_grid_out;

    double palmbase_x, palmbase_y;
    double indexbase_x, indexbase_y;

    cv::Scalar color_red, color_blue, color_green, color_cur;

    int m_selected_i, m_selected_j, m_selected_i_prv, m_selected_j_prv; 

    int m_marked_i, m_marked_j;
    
    std::string message, message_selected;

    bool m_static_display; // if true then keep on showing display

    int progress_maxwidth, progress_maxheight; // progress bar
    int pwidth, npwidth;

    ScreenSize m_screen;

    bool m_selection_locked;

    std::vector<std::pair<int, int>> m_past_selections;
    int m_cur_time_id, m_past_selections_size;
    
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

    void highlightSelected();
    void setDivisions(int _divisions);
    int getDivisions();
    void drawProgressBar(cv::Mat & _image, Parameters & params);

    void setupGrid(double enlarged_topleft_x, double enlarged_topleft_y);
    void setupSelection(int index_pointer_x, int index_pointer_y, 
        int & selected_row_i, int & selected_col_j);
    
    void drawTextHighlighted(cv::Mat & overlay);
    void drawTextSelected(cv::Mat & overlay);
    
    void reset_palmbase();
    void reset_indexbase();
    void reset_grids();

    void setScreenSize(const choices::eScreenSize & size);

    void setVisibility(const choices::eVisibility & _visibility);
    choices::eVisibility getVisibility();
    bool isVisible(const Parameters & params);

    choices::anchor::types type();

    void lock_selection();
    void unlock_selection();
    void draw_main_grid_layout(cv::Mat & src, const Grid & grid);
    void draw_cells(cv::Mat & src, const Grid & grid);

    bool is_selection_changed();
    void adjust_selection_prior_trigger();

    void reset_selection();
    void reset_marked_cell();
    void setup_background(
        cv::Mat & _background, 
        std::string _imagePath, 
        int _width, 
        int _height);
    
    bool is_anycell_marked();
};


#endif