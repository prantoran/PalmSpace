
#include "anchors.h"


AnchorPadLarge::~AnchorPadLarge() {
    std::cout << "anchor static killed\n";
}


AnchorPadLarge::AnchorPadLarge() {
    initiate();
}


AnchorPadLarge::AnchorPadLarge(
    const int _width,
    const int _height,
    const cv::Scalar & red, 
    const cv::Scalar & blue, 
    const std::string & imagePath,
    const std::string & imagePathBackground) {
    
    initiate();

    color_red = red;
    color_blue = blue;

    setup_palmiamge(imagePath, _width, _height);
    setup_background(m_background, imagePathBackground, _width, _height);





    m_grid_out.m_width_min  = std::max(0, (0*image_palm.cols/150) + ((2*image_palm.cols)/5));
    m_grid_out.m_height_min = std::max(0, (image_palm.rows/10) + (image_palm.rows/2));

    std::cerr << "m_grid_out width:" << m_grid_out.m_width_min << "\theight:" << m_grid_out.m_height_min << "\n";
}


void AnchorPadLarge::initiate() {
    name = "padlarge";
    m_type = choices::anchor::PADLARGE;

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

    indexbase_x = 0, indexbase_y = 0;
    palmbase_x = 0, palmbase_y = 0;
    m_indexbase = SmoothCoord("indexbase", 0.9, 240, 15);

    m_selection_locked = false;
    
    reset_grids();

    m_visited_cells = 0;
}


void AnchorPadLarge::setup_palmiamge(std::string imagePath, int _width, int _height) {
    // only take .png as it has alpha channel for transparency
    image_palm = cv::imread(imagePath, CV_LOAD_IMAGE_UNCHANGED); 
    // CV_LOAD_IMAGE_COLOR ignores alpha transparency channel
    // https://docs.opencv.org/3.4/da/d0a/group__imgcodecs__c.html
    cv::flip(image_palm, image_palm, 1); // flip vertically

    if(image_palm.channels() < 4) {
        return;
    }

    cv::resize(image_palm, image_palm, cv::Size(_width, _height));
    
    // https://answers.opencv.org/question/174551/how-to-show-transparent-images/
    std::vector<cv::Mat> rgbLayer;

    split(image_palm, rgbLayer);
    cv::Mat cs[3] = { rgbLayer[0],rgbLayer[1],rgbLayer[2] };
    cv::merge(cs, 3, image_palm);  // glue together again
    mask = rgbLayer[3];       // png's alpha channel used as mask
}



void AnchorPadLarge::calculate(
    const cv::Mat& input,
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {
    
    if (!width || !height) {
        setConfig(input.size().width, input.size().height);
        m_grid_out.align((image_palm.cols/40) + image_palm.cols/5, params.m_frame_height-(5*image_palm.rows/7));

    }
    
    {   // adaptive input surface    
        m_palm_x = params.palm_width();
        m_palm_y = params.palm_height();
        
        m_grid.m_width_min  = std::min((double)params.m_frame_width, m_palm_x.second  - m_palm_x.first);
        m_grid.m_height_min = std::min((double)params.m_frame_height, m_palm_y.second - m_palm_y.first);
    }

    indexbase_x = params.m_indexbase.x();
    indexbase_y = params.m_indexbase.y();

    if (indexbase_x >= 0 && indexbase_y >= 0) {
        m_static_display = true;

        m_indexbase.set(
            std::max(0.0, (double)(m_palm_x.first)),
            std::max(0.0, (double)(m_palm_y.first))
        );

        indexbase_x = m_indexbase.x();
        indexbase_y = m_indexbase.y();
        
        std::cout << "index x:" << indexbase_x << "\ty:" << indexbase_y << "\tpx:" << (m_palm_x.first) << "\tpy:" << (m_palm_x.first)<< "\n";

        m_grid.align(indexbase_x, indexbase_y);
    
        setupSelection(pointer_x, pointer_y, m_selected_i, m_selected_j); // defined in parent anchor class
        // std::cout << "selected_i:" << m_selected_i << "\t j:" << m_selected_j << "\n";
        params.set_selected_cell(m_selected_i, m_selected_j);
    } else {
        m_indexbase.reset();
    }
}


void AnchorPadLarge::draw(
    const cv::Mat& input, 
    cv::Mat& output,
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {
        
    // cv::Mat overlay;
    // input.copyTo(overlay);
    cv::Mat overlay = cv::Mat(
        input.rows,
        input.cols,
        CV_8UC3,
        cv::Scalar(0, 0, 0)
    );

    m_background.copyTo(output);

    if (params.m_palmbase.x() > 0) { // putting palm when palm coord detected
        image_palm.copyTo(
            output(
                cv::Rect(
                    0,
                    params.m_frame_height-image_palm.rows, 
                    image_palm.cols, 
                    image_palm.rows
                )
            ), 
            mask
        );
    }
    
    draw_main_grid_layout(overlay, m_grid_out);

    draw_cells(overlay, m_grid_out);

    drawTextHighlighted(overlay);
    drawTextMarked(overlay);

    drawProgressBar(overlay, params);

    cv::addWeighted(overlay, TRANSPARENCY_ALPHA, output, 1-TRANSPARENCY_ALPHA, 0, output);
    // cv::addWeighted(overlay, alpha, input, 1-alpha, 0, output);
}

