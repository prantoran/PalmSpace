#include "anchors.h"
 
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

#include <vector>


std::string AnchorStatic::type() {
    return "anchor-type=static";
}


AnchorStatic::~AnchorStatic() {
    std::cout << "anchor static killed\n";
}


AnchorStatic::AnchorStatic() {
    name = "static";
    ws = 0;
    hs = 0;

    width = 0;
    height = 0;
    gap = 15;


    color_red = cv::Scalar(25, 25, 255);
    color_blue = cv::Scalar(255, 25, 25);
    color_green = cv::Scalar(25, 255, 25);


    selected_i_prv = -1, selected_j_prv = -1;
    selected_i = -1, selected_j = -1;

    static_display = false;

    palm_ubx = 0, palm_uby = 0;

    reset_grid();
}


AnchorStatic::AnchorStatic(cv::Scalar red, cv::Scalar blue, std::string img_path) {
    width = 0;
    height = 0;
    gap = 5;

    color_red = red;
    color_blue = blue;
    color_green = cv::Scalar(25, 255, 25);

    selected_i_prv = -1, selected_j_prv = -1;
    selected_i = -1, selected_j = -1;
    static_display = false;
    green_i = -1, green_j = -1;

    // only take .png as it has alpha channel for transparency
    // image_palm = cv::imread("/home/prantoran/work/src/github.com/google/fresh/mediapipe/desktop/anchors/Hand.png", CV_LOAD_IMAGE_COLOR); // ignores alpha transparency channel
    image_palm = cv::imread(img_path, CV_LOAD_IMAGE_UNCHANGED);
    // https://docs.opencv.org/3.4/da/d0a/group__imgcodecs__c.html

    cv::resize(image_palm, image_palm, cv::Size(300,300));
    
    // https://answers.opencv.org/question/174551/how-to-show-transparent-images/
    std::vector<cv::Mat> rgbLayer;
    if(image_palm.channels() < 4) {
        return;
    }

    split(image_palm, rgbLayer);
    cv::Mat cs[3] = { rgbLayer[0],rgbLayer[1],rgbLayer[2] };
    cv::merge(cs, 3, image_palm);  // glue together again
    mask = rgbLayer[3];       // png's alpha channel used as mask

    // cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    // cv::imshow("Display window", mask);                   // Show our image inside it.

    // cv::waitKey(0);

    palm_ubx = 0, palm_uby = 0;

    reset_grid();
}


void AnchorStatic::calculate(
    const cv::Mat& input, 
    const std::tuple<double, double, double> & palmbase,
    const std::tuple<double, double, double> & indexbase, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    std::vector<double> & extra_params) {

    if (!width || !height) {
        setConfig(input.size().width, input.size().height);
        
        static_display = true;

        min_ws = width;
        min_hs = height;

        palmbase_x = 0.5; // determines a position of grid
        palmbase_y = 1;   // middle-bottom point

    }

    setupGrid((palmbase_x*width) - (ws/2), (palmbase_y*height) - hs); // defined in parent anchor class
    
    double palmbase_x_new = std::get<0>(palmbase);
    double palmbase_y_new = std::get<1>(palmbase);

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

    setupSelection(pointer_x, pointer_y); // defined in parent anchor class
    
    std::cout << "anchors_static selected i:" << selected_i << "\tj:" << selected_j << "\n";
    if (green_i != -1 && green_j != -1) {
        double gdx = xs[green_i] - palmstart_x;
        double gdy = ys[green_j] - palmstart_y;
        if (gdx < 50 || gdx > 250 || gdy < 50 || gdy > 250) {
           // invalidating pointer_x for pointers outside the palm image
            green_i = -1;
            green_j = -1;
        }
    }

    if (extra_params.size() > 7) {
      extra_params[7] = selected_i;
      extra_params[8] = selected_j;
    } else {
    }

}


void AnchorStatic::draw(
    cv::Mat& input, 
    const std::tuple<double, double, double> & palmbase,
    const std::tuple<double, double, double> & indexbase, 
    double scale_ratio, 
    int pointer_x, int pointer_y,
    std::vector<double> & extra_params) {
    
    double palmbase_x_new = std::get<0>(palmbase);
    double palmbase_y_new = std::get<1>(palmbase);
     
    if (palmbase_x_new != -1) { // putting palm when palm coord detected
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

        cv::Point p1(palmstart_x+50, palmstart_y+50);                            // start & end points 
        cv::Point p2(palmstart_x+50,palmstart_y+250);
        cv::Point p3(palmstart_x+250,palmstart_y+250);                            // start & end points 
        cv::Point p4(palmstart_x+250,palmstart_y+50);
        
        std::vector<cv::Point> contour = {p1, p2, p3, p4, p1};

        const cv::Point *pts = (const cv::Point*) cv::Mat(contour).data;
        int npts = cv::Mat(contour).rows;

        cv::polylines(
            input, 
            &pts, 
            &npts, 
            1, 
            false, 
            COLORS_floralwhite, 
            3,
            CV_AA,
            0
        );
    
    }

    cv::Mat overlay;
    input.copyTo(overlay);

    cv::rectangle(
        overlay, 
        getGridTopLeft(), getGridBottomRight(), 
        cv::Scalar(25, 25, 125),
        -1, 
        cv::LINE_8,
        0);

    for (int i = 1; i <= divisions; i ++ ) {
        for (int j = 1; j <= divisions; j ++) {
          color_cur = color_red;
          if (i == green_i && j == green_j) {
            color_cur = color_green;
          } else if (i == selected_i && j == selected_j) {
            color_cur = color_blue;
          }

          cv::rectangle(
            overlay, 
            cv::Point(xs[i], ys[j]), cv::Point(xs[i]+dx, ys[j]+dy), 
            color_cur,
            -1, 
            cv::LINE_8,
            0);
        }
    }

    drawTextHighlighted(overlay);
    drawTextSelected(overlay);

    drawProgressBar(overlay, extra_params[9]);

    cv::addWeighted(overlay, alpha, input, 1-alpha, 0, input);
}

