#include "anchors.h"


AnchoHandToScreen::~AnchoHandToScreen() {
    std::cout << "anchor AnchoHandToScreen killed\n";
}


AnchoHandToScreen::AnchoHandToScreen() {
    initiate();
}


AnchoHandToScreen::AnchoHandToScreen(
    const int _width,
    const int _height,
    const cv::Scalar & red, 
    const cv::Scalar & blue, 
    const std::string & imagePath,
    const std::string & imagePathBackground) {
    
    initiate();

    color_red = red;
    color_blue = blue;

    setup_palmiamge(imagePath);
    setup_background(m_background, imagePathBackground, _width, _height);
}


void AnchoHandToScreen::initiate() {
    name = "handtoscreen";
    m_type = choices::anchor::HANDTOSCREEN;

    width = 0;
    height = 0;
 
    m_grid.reset_minimum_dimensions();
    m_grid_out.reset_minimum_dimensions();

    color_red = COLORS_red;
    color_blue = COLORS_blue;
    color_green = COLORS_darkgreen;

    reset_selection();
    
    reset_marked_cell();

    m_static_display = false;

    palm_ubx = 0, palm_uby = 0; // used as bounds for palm image

    indexbase_x = 0, indexbase_y = 0;
    palmbase_x = 0, palmbase_y = 0;
    // m_indexbase = SmoothCoord("indexbase", 0.9, 240, 15);

    m_selection_locked = false;
    
    reset_grids();
}

void AnchoHandToScreen::setup_palmiamge(std::string imagePath) {
    // only take .png as it has alpha channel for transparency
    image_palm = cv::imread(imagePath, CV_LOAD_IMAGE_UNCHANGED); 
    // CV_LOAD_IMAGE_COLOR ignores alpha transparency channel
    // https://docs.opencv.org/3.4/da/d0a/group__imgcodecs__c.html
    cv::flip(image_palm, image_palm, 1); // flip vertically

    if(image_palm.channels() < 4) {
        return;
    }

    cv::resize(image_palm, image_palm, cv::Size(250,250));
    
    // https://answers.opencv.org/question/174551/how-to-show-transparent-images/
    std::vector<cv::Mat> rgbLayer;

    split(image_palm, rgbLayer);
    cv::Mat cs[3] = { rgbLayer[0],rgbLayer[1],rgbLayer[2] };
    cv::merge(cs, 3, image_palm);  // glue together again
    mask = rgbLayer[3];       // png's alpha channel used as mask
}


void AnchoHandToScreen::calculate(
    const cv::Mat& input,
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {
        
    std::cerr << "anchor hand2screen calculate()\n";

    if (!width || !height) {
        setConfig(input.size().width, input.size().height);
                

        m_screen.setMinWidthHeight(
            m_grid.m_width_min, 
            m_grid.m_height_min,
            width, 
            height
        );
        
        // m_grid.m_width_min  = image_palm.cols;
        // m_grid.m_height_min = image_palm.rows;

        m_screen.setMinWidthHeight(
            m_grid_out.m_width_min, 
            m_grid_out.m_height_min, 
            width, 
            height
        );
    }

    if (!m_static_display) {
        indexbase_x = params.m_indexbase.x();
        indexbase_y = params.m_indexbase.y();

        if (indexbase_x > 0 && indexbase_y > 0) {
            m_static_display = true;
  
            if (m_screen.isFull()) {
                palmbase_x = 0.5; // determines a position of grid
                palmbase_y = 1;   // middle-bottom point

                indexbase_x = 0;
                indexbase_y = 0;
            } else if (m_screen.isCentered()){
                // fixing at center
                indexbase_x = ((double)(width - m_grid_out.m_width_min) / 2) / width;
                indexbase_y = ((double)(height - m_grid_out.m_height_min) / 2) / height;
            } else {
                // adjusting indexbase if grid goes out of bound
                if (indexbase_x*width + m_grid_out.m_width_min >= width) {
                    indexbase_x = (double)(width-m_grid_out.m_width_min)/width;
                }

                if (indexbase_y*height + m_grid_out.m_height_min >= height) {
                    indexbase_y = (double)(height-m_grid_out.m_height_min)/height;
                }
            }
        }
    } 

    // reset m_static_display if visibility not fixed and 0 hands detected.
    m_static_display &= isVisible(params);
    
    if (m_static_display) {
        // setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
        
        m_grid.align(indexbase_x*width, indexbase_y*height);
        // align_grid(m_grid, params.m_indexbase);
        m_grid_out.align(indexbase_x*width, indexbase_y*height);

        setupInputGrid(pointer_x, pointer_y, params.m_palmbase);

        if (!isInside(pointer_x, pointer_y)) {
            pointer_x = 0; // resetting cursor
            pointer_y = 0;
        }

        setupSelection(pointer_x, pointer_y, m_selected_i, m_selected_j); // defined in parent anchor class
        
        params.set_selected_cell(m_selected_i, m_selected_j);
    }
}


void AnchoHandToScreen::align_grid(Grid & _grid, const SmoothCoord & _coord) {
    _grid.align(
        _coord.x() * width,
        _coord.y() * height
    );
}


void AnchoHandToScreen::setupInputGrid(
    int cursor_x, 
    int cursor_y,
    const SmoothCoord & base
) {
    double base_x_new = base.x();
    double base_y_new = base.y();

    if (base_x_new != -1) { // putting palm when palm coord detected
        m_inputgrid_topleft_x = base_x_new*width  - image_palm.cols/2;
        m_inputgrid_topleft_y = base_y_new*height - image_palm.rows;

        if (m_inputgrid_topleft_x < 0) m_inputgrid_topleft_x = 0;
        if (m_inputgrid_topleft_y < 0) m_inputgrid_topleft_y = 0;

        if (!palm_ubx) {
            palm_ubx = width  - image_palm.cols;
            palm_uby = height - image_palm.rows;
        }

        if (m_inputgrid_topleft_x >= palm_ubx) {
            m_inputgrid_topleft_x = palm_ubx;
        }

        if (m_inputgrid_topleft_y >= palm_uby) {
            m_inputgrid_topleft_y = palm_uby;
        }
    }


    m_input_bound.m_topleft = cv::Point(
        m_inputgrid_topleft_x + image_palm.cols/6, 
        m_inputgrid_topleft_y + (2*image_palm.rows)/5
    );

    m_input_bound.m_bottomright = cv::Point(
        m_inputgrid_topleft_x + (4*image_palm.cols)/6, 
        m_inputgrid_topleft_y + image_palm.rows - image_palm.rows/15
    );
}


bool AnchoHandToScreen::isInside(int cursor_x, int cursor_y) const {
    if (cursor_x < (m_input_bound.m_topleft.x - m_grid.m_dx_col) || 
            cursor_x > (m_input_bound.m_bottomright.x + m_grid.m_dx_col)) {
        return false;
    }

    if (cursor_y < (m_input_bound.m_topleft.y - m_grid.m_dy_row) || 
            cursor_y > (m_input_bound.m_bottomright.y + m_grid.m_dy_row)) {
        return false;
    }

    return true;
}


void AnchoHandToScreen::draw(
    const cv::Mat& input, 
    cv::Mat& output,
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {

    m_background.copyTo(output);

    cv::Mat overlay = cv::Mat(
        input.rows,
        input.cols,
        CV_8UC3,
        cv::Scalar(0, 0, 0)
    );

    if (params.m_palmbase.x() > 0) { // putting palm when palm coord detected
        image_palm.copyTo(
            output(
                cv::Rect(
                    m_inputgrid_topleft_x,
                    m_inputgrid_topleft_y, 
                    image_palm.cols, 
                    image_palm.rows
                )
            ), 
            mask
        );
        
        ui::clear_rect(
            output,
            m_input_bound.m_topleft,
            m_input_bound.m_bottomright,
            COLORS_floralwhite
        );
    }


    draw_main_grid_layout(overlay, m_grid_out);

    draw_cells(overlay, m_grid_out);

    drawTextHighlighted(overlay);
    drawTextSelected(overlay);

    drawProgressBar(overlay, params);

    cv::addWeighted(overlay, TRANSPARENCY_ALPHA, output, 1-TRANSPARENCY_ALPHA, 0, output);
}

