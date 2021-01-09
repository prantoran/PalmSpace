 #include "anchors.h"


AnchorMidAir::~AnchorMidAir() {
    std::cout << "anchor midair killed\n";
}

AnchorMidAir::AnchorMidAir() {
    initiate();
    reset_grid();
}


AnchorMidAir::AnchorMidAir(
    const cv::Scalar & red, 
    const cv::Scalar & blue) {
    
    initiate();

    color_red = red;
    color_blue = blue;

    reset_grid();
}


void AnchorMidAir::initiate() {
    name = "midair";
    _type = choices::anchor::MIDAIR;
    
    width = 0;
    height = 0;

    m_grid.reset_dimensions();
  
    color_red = COLORS_red;
    color_blue = COLORS_blue;
    color_green = COLORS_darkgreen;

    m_selected_i_prv = -1, m_selected_j_prv = -1;
    m_selected_i = -1, m_selected_j = -1;
    green_i = -1, green_j = -1;

    static_display = false;

    indexbase_x = -1, indexbase_y = -1;
    palmbase_x = -1, palmbase_y = -1;
}


void AnchorMidAir::calculate(
    const cv::Mat& input, 
    const std::tuple<double, double, double> & palmbase,
    const std::tuple<double, double, double> & indexbase, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {

    if (!width || !height) {
        setConfig(input.size().width, input.size().height);

        screen.setMinWidthHeight(
            m_grid.m_width_min, m_grid.m_height_min, 
            width, height);
    }
    
    // if (std::get<0>(palmbase) >= 0 && std::get<1>(palmbase) >= 0) {
    //     palmbase_x = 0.5; // determines a position of grid
    //     palmbase_y = 1;   // middle-bottom point

    //     static_display = true;
    // }

    // setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
    if (!static_display) {
        indexbase_x = std::get<0>(indexbase);
        indexbase_y = std::get<1>(indexbase);

        if (indexbase_x > 0 && indexbase_y > 0) {
            static_display = true;
  
            if (screen.isFull()) {
                palmbase_x = 0.5; // determines a position of grid
                palmbase_y = 1;   // middle-bottom point

                indexbase_x = 0;
                indexbase_y = 0;
            } else if (screen.isCentered()){
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
    
    // reset static_display if visibility not fixed and 0 hands detected.
    static_display &= isVisible(params);
    
    if (static_display) {
        // setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
        setupGrid(indexbase_x*width, indexbase_y*height); // defined in parent anchor class

        std::cout << "anchor_midair pointer x:" << pointer_x << " y:" << pointer_y << "\n";
        setupSelection(pointer_x, pointer_y, m_selected_i, m_selected_j); // defined in parent anchor class

        std::cout << "anchor_midair selected i:" << m_selected_i << "\tj:" << m_selected_j << "\n";

        params.set_selected_cell(m_selected_i, m_selected_j);
    }
}

void AnchorMidAir::draw(
    const cv::Mat& input, 
    cv::Mat& output, 
    const std::tuple<double, double, double> & palmbase,
    const std::tuple<double, double, double> & indexbase, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {
    
    cv::Mat overlay;
    // input.copyTo(overlay);

    try {
        overlay = cv::Mat(
            input.rows, 
            input.cols, 
            CV_8UC3, 
            cv::Scalar(0, 0, 0));

    } catch(cv::Exception e) {
        std::cerr << "ERROR anchors/anchors_static.cpp draw() " << e.what() << "\n";
    }
    // input.copyTo(overlay);

    cv::rectangle(
        overlay, 
        getGridTopLeft(), getGridBottomRight(), 
        cv::Scalar(25, 25, 125),
        -1, 
        cv::LINE_8,
        0);

    for (int i = 1; i <= m_grid.m_divisions; i ++ ) {
        for (int j = 1; j <= m_grid.m_divisions; j ++) {
          color_cur = color_red;
          if (i == green_i && j == green_j) {
            color_cur = color_green;
          } else if (i == m_selected_i && j == m_selected_j) {
            color_cur = color_blue;
          }

          cv::rectangle(
            overlay, 
            m_grid.get_cell(i, j), 
            color_cur,
            -1, 
            cv::LINE_8,
            0);
        }
    }

    drawTextHighlighted(overlay);
    drawTextSelected(overlay);

    drawProgressBar(overlay, params);

    cv::addWeighted(overlay, 0.7, input, 1-0.7, 0, output);
}
