#include "h2s_relative.h"


AnchorH2SRelative::~AnchorH2SRelative() {
  std::cout << "anchor " << name << " killed\n";
}


AnchorH2SRelative::AnchorH2SRelative() {
  initiate();
  color_red  = cv::Scalar(25, 25, 255);
  color_blue = cv::Scalar(255, 25, 25);
}


AnchorH2SRelative::AnchorH2SRelative(
    const int _width,
    const int _height,
    const cv::Scalar & red, 
    const cv::Scalar & blue,
    const std::string & imagePath,
    const std::string & imagePathBackground) {
  
    initiate();
    color_red = red;
    color_blue = blue;

    m_image_path = imagePath;
    setup_palmiamge(m_image_path, 250, 250);
    setup_background(m_background, imagePathBackground, _width, _height);


}


void AnchorH2SRelative::initiate() {
  name = "h2s_relative";
  m_type = choices::anchor::H2SRELATIVE;
  
  width = 0;
  height = 0;

  m_grid.reset_dimensions();
  m_grid_out.reset_dimensions();
  
  color_green = COLORS_darkgreen;

  reset_selection();
  
  reset_marked_cell();

  m_static_display = false;

  palmbase_x = 0;
  palmbase_y = 0;

  indexbase_x = 0;
  indexbase_y = 0;

  m_selection_locked = false;

  reset_grids();
  reset_palmbase();
  reset_palmbase_right();
  reset_indexbase();

  m_visited_cells = 0;
}


void AnchorH2SRelative::setup_palmiamge(std::string imagePath, int x_cols, int y_rows) {
    // only take .png as it has alpha channel for transparency
    image_palm = cv::imread(imagePath, CV_LOAD_IMAGE_UNCHANGED); 
    // CV_LOAD_IMAGE_COLOR ignores alpha transparency channel
    // https://docs.opencv.org/3.4/da/d0a/group__imgcodecs__c.html
    cv::flip(image_palm, image_palm, 1); // flip vertically

    if(image_palm.channels() < 4) {
        return;
    }

    cv::resize(image_palm, image_palm, cv::Size(x_cols, y_rows));
    
    // https://answers.opencv.org/question/174551/how-to-show-transparent-images/
    std::vector<cv::Mat> rgbLayer;

    split(image_palm, rgbLayer);
    cv::Mat cs[3] = { rgbLayer[0],rgbLayer[1],rgbLayer[2] };
    cv::merge(cs, 3, image_palm);  // glue together again
    mask = rgbLayer[3];       // png's alpha channel used as mask
}

void AnchorH2SRelative::calculate(
    const cv::Mat& input, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    Parameters & params) {
    
    m_calculate_done = false;

    if (!width || !height) {
      setConfig(input.size().width, input.size().height);
      
      m_screen.setMinWidthHeight(
        m_grid.m_width_min, 
        m_grid.m_height_min, 
        width, 
        height
      );

      m_screen.setMinWidthHeight(
        m_grid_out.m_width_min, 
        m_grid_out.m_height_min, 
        width, 
        height
      ); 

      m_image_palm_dim_max = (double)std::min(width, height)/3;
    }
    
    if (m_inputspace_type == choices::inputspace::PALMSIZED) {
      m_palm_x = params.palm_width();
      m_palm_y = params.palm_height();

      m_max_dim = std::min(
                      m_image_palm_dim_max, 
                      std::max(
                          64.0, 
                          std::max(
                              m_palm_x.second  - m_palm_x.first, 
                              m_palm_y.second - m_palm_y.first
                          )
                      )
                  );
      
      m_grid.m_width_min  = std::max(
                              64.0, 
                              std::min(
                                (double)params.m_frame_width, 
                                m_max_dim
                              )
                            );

      m_grid.m_height_min = std::max(
                              64.0, 
                              std::min(
                                (double)params.m_frame_height, 
                                m_max_dim
                              )
                            );

      m_grid_out.m_width_min = m_grid.m_width_min;
      m_grid_out.m_height_min = m_grid.m_height_min;

      setup_palmiamge(
        m_image_path, 
        std::min((double)(width-50), (5*m_max_dim)/2), 
        std::min((double)(height-50), (m_max_dim*19)/8)
      );

    } 

    updateBase(params.m_palmbase, palmbase_x, palmbase_y);
    updateBase(params.m_indexbase, indexbase_x, indexbase_y);

    indexbase_x *= width;
    indexbase_y *= height;

    if (m_inputspace_type == choices::inputspace::PALMSIZED) {
      indexbase_x = m_palm_x.first;
      indexbase_y = m_palm_y.first;
    }
    
    if (indexbase_x > 0 && indexbase_y > 0) {
      m_grid.align(indexbase_x, indexbase_y);
      m_grid_out.align(indexbase_x, indexbase_y);
      
      const cv::Point & g = m_grid.get_bottom_middle();

      setupInputGrid(pointer_x, pointer_y, {g.x, g.y});
      
      m_grid.align(
        m_inputgrid_topleft_x + image_palm.cols/5, 
        m_inputgrid_topleft_y + image_palm.rows/2
      );

      m_grid_out.align(
        m_inputgrid_topleft_x + image_palm.cols/4, 
        m_inputgrid_topleft_y + image_palm.rows/2 
      );
      


      setupSelection(pointer_x, pointer_y, m_selected_i, m_selected_j); // defined in parent anchor class
      
      params.set_selected_cell(m_selected_i, m_selected_j);
    }

    m_calculate_done = true;
}


void AnchorH2SRelative::setupInputGrid(
    int cursor_x, 
    int cursor_y,
    const std::pair<double, double> & base
) {
    double base_x_new = base.first;
    double base_y_new = base.second;

    if (base_x_new != -1) { // putting palm when palm coord detected
        m_inputgrid_topleft_x = base_x_new  - image_palm.cols/2 + image_palm.cols/10;
        m_inputgrid_topleft_y = base_y_new - image_palm.rows + image_palm.rows/9;

        if (m_inputgrid_topleft_x < 0) m_inputgrid_topleft_x = 0;
        if (m_inputgrid_topleft_y < 0) m_inputgrid_topleft_y = 0;

        palm_ubx = std::min(width,  std::max(0, width  - image_palm.cols));
        palm_uby = std::min(height, std::max(0, height - image_palm.rows));

        if (m_inputgrid_topleft_x >= palm_ubx) {
            m_inputgrid_topleft_x = palm_ubx;
        }

        if (m_inputgrid_topleft_y >= palm_uby) {
            m_inputgrid_topleft_y = palm_uby;
        }
    }
}


void AnchorH2SRelative::updateBase(const SmoothCoord & base, double & _x, double & _y) {
  _x = base.x();
  _y = base.y();
}


void AnchorH2SRelative::draw(
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

    if (
      (m_inputgrid_topleft_x >= 0 && m_inputgrid_topleft_x < width-image_palm.cols) &&
      (m_inputgrid_topleft_y >= 0 && m_inputgrid_topleft_y < height-image_palm.rows)){ // putting palm when palm coord 
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
    }

    draw_main_grid_layout(overlay, m_grid_out);
    
    draw_cells(overlay, m_grid_out);

    drawTextHighlighted(overlay);
    drawTextMarked(overlay);

    drawProgressBar(overlay, params);
    
    cv::addWeighted(overlay, TRANSPARENCY_ALPHA, output, 1-TRANSPARENCY_ALPHA, 0, output);
}

