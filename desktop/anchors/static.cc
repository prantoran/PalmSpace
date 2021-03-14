#include "anchors.h"


AnchorStatic::~AnchorStatic() {
    std::cout << "anchor static killed\n";
}


AnchorStatic::AnchorStatic() {
    initiate();
}


AnchorStatic::AnchorStatic(
    const cv::Scalar & red, 
    const cv::Scalar & blue, 
    const std::string & imagePath) {
    
    initiate();

    color_red = red;
    color_blue = blue;

    setup_palmiamge(imagePath);

}


void AnchorStatic::initiate() {
    name = "static";
    m_type = choices::anchor::STATIC;

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

    indexbase_x = -1, indexbase_y = -1;
    palmbase_x = -1, palmbase_y = -1;

    m_selection_locked = false;
    
    reset_grids();

    m_visited_cells = 0;
}

void AnchorStatic::setup_palmiamge(std::string imagePath) {
    // only take .png as it has alpha channel for transparency
    image_palm = cv::imread(imagePath, CV_LOAD_IMAGE_UNCHANGED); 
    // CV_LOAD_IMAGE_COLOR ignores alpha transparency channel
    // https://docs.opencv.org/3.4/da/d0a/group__imgcodecs__c.html

    if(image_palm.channels() < 4) {
        return;
    }

    cv::resize(image_palm, image_palm, cv::Size(300,300));
    
    // https://answers.opencv.org/question/174551/how-to-show-transparent-images/
    std::vector<cv::Mat> rgbLayer;

    split(image_palm, rgbLayer);
    cv::Mat cs[3] = { rgbLayer[0],rgbLayer[1],rgbLayer[2] };
    cv::merge(cs, 3, image_palm);  // glue together again
    mask = rgbLayer[3];       // png's alpha channel used as mask

    // cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    // cv::imshow("Display window", mask);                   // Show our image inside it.
    // cv::waitKey(0);
}



void AnchorStatic::calculate(
    const cv::Mat& input,
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {
        
    if (!width || !height) {
        setConfig(input.size().width, input.size().height);
                
        m_screen.setMinWidthHeight(
            m_grid.m_width_min, m_grid.m_height_min, 
            width, height);
        
        m_screen.setMinWidthHeight(
            m_grid_out.m_width_min, m_grid_out.m_height_min,
            width, height);
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
                indexbase_x = ((double)(width - m_grid.m_width_min) / 2) / width;
                indexbase_y = ((double)(height - m_grid.m_height_min) / 2) / height;
            } else {
                if (indexbase_x*width + m_grid.m_width_min >= width) {
                    indexbase_x = (double)(width-m_grid.m_width_min)/width;
                }

                if (indexbase_y*height + m_grid.m_height_min >= height) {
                    indexbase_y = (double)(height-m_grid.m_height_min)/height;
                }
            }
        }
    } 

    // reset m_static_display if visibility not fixed and 0 hands detected.
    m_static_display &= isVisible(params);
    
    if (m_static_display) {
        // setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
        
        m_grid.align(indexbase_x*width, indexbase_y*height);
        m_grid_out.align(indexbase_x*width, indexbase_y*height);

        checkSelectionWithinPalm(pointer_x, pointer_y, params.m_palmbase);

        setupSelection(pointer_x, pointer_y, m_selected_i, m_selected_j); // defined in parent anchor class
        
        if (m_marked_i != -1 && m_marked_j != -1) {
            // ensureMarkedCellWithinPalm(m_marked_i, m_marked_j);
        }

        params.set_selected_cell(m_selected_i, m_selected_j);
    }
}


void AnchorStatic::ensureMarkedCellWithinPalm(int & marked_row_i, int & marked_col_j) {
    double gdx = m_grid.m_x_cols[marked_row_i] - palmstart_x;
    double gdy = m_grid.m_y_rows[marked_col_j] - palmstart_y;
    if (gdx < 50 || gdx > 250 || gdy < 50 || gdy > 250) {
    // invalidating pointer_x for pointers outside the palm image
        marked_row_i = -1;
        marked_col_j = -1;
    }
}


void AnchorStatic::checkSelectionWithinPalm(
    int pointer_x, int pointer_y,
    const SmoothCoord & palmbase) {

    double palmbase_x_new = palmbase.x();
    double palmbase_y_new = palmbase.y();

    if (palmbase_x_new != -1) { // putting palm when palm coord detected
        palmstart_x = palmbase_x_new*width - 150;
        palmstart_y = palmbase_y_new*height - 300;

        if (palmstart_x < 0) palmstart_x = 0;
        if (palmstart_y < 0) palmstart_y = 0;

        if (!palm_ubx) {
            palm_ubx = width - 300;
            palm_uby = height - 300;
        }

        if (palmstart_x >= palm_ubx) {
            palmstart_x = palm_ubx;
        }

        if (palmstart_y >= palm_uby) {
            palmstart_y = palm_uby;
        }
    }

    double dx = pointer_x-palmstart_x;
    double dy = pointer_y-palmstart_y;
    if (dx < 50 || dx > 250 || dy < 50 || dy > 250) {
        // invalidating pointer_x for pointers outside the palm image
        pointer_x = -1;
    }
}


void AnchorStatic::draw(
    const cv::Mat& input, 
    cv::Mat& output,
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {
        
    cv::Mat overlay;
    input.copyTo(overlay);

    if (m_screen.isFull()) { 
        if (params.m_palmbase.x() != -1) { // putting palm when palm coord detected
            image_palm.copyTo(
                input(
                    cv::Rect(
                        palmstart_x,
                        palmstart_y, 
                        image_palm.cols, 
                        image_palm.rows
                    )
                ), 
                mask
            );
            
            ui::clear_rectangle(
                overlay,
                cv::Point(palmstart_x+50, palmstart_y+50),                            // start & end points 
                cv::Point(palmstart_x+50,palmstart_y+250),
                cv::Point(palmstart_x+250,palmstart_y+250),                            // start & end points 
                cv::Point(palmstart_x+250,palmstart_y+50),
                COLORS_floralwhite
            );
        }
    }

    draw_main_grid_layout(overlay, m_grid_out);

    draw_cells(overlay, m_grid_out);

    drawTextHighlighted(overlay);
    drawTextSelected(overlay);

    drawProgressBar(overlay, params);

    cv::addWeighted(overlay, TRANSPARENCY_ALPHA, input, 1-TRANSPARENCY_ALPHA, 0, output);
}

