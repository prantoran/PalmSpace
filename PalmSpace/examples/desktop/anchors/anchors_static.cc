#include "anchors.h"
 
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

std::string AnchorStatic::type() {
    return "anchor-type=dynamic";
}

AnchorStatic::AnchorStatic() {
    name = "static";

    width = 0;
    height = 0;
    palmbase_momentum = 0.9;
    gap = 15;

    color_red = cv::Scalar(25, 25, 255);
    color_blue = cv::Scalar(255, 25, 25);
    color_green = cv::Scalar(25, 255, 25);

    selected_i_prv = -1, selected_j_prv = -1;
    selected_i = -1, selected_j = -1;

    static_display = false;
}

AnchorStatic::AnchorStatic(cv::Scalar red, cv::Scalar blue) {
    width = 0;
    height = 0;
    palmbase_momentum = 0.9;
    gap = 15;

    color_red = red;
    color_blue = blue;
    color_green = cv::Scalar(25, 255, 25);

    selected_i_prv = -1, selected_j_prv = -1;
    selected_i = -1, selected_j = -1;
    static_display = false;
}


cv::Mat AnchorStatic::transform(
    const cv::Mat& input, 
    double palmbase_x_new, double palmbase_y_new, 
    double interface_scaling_factor, 
    int pointer_x, int pointer_y,
    const std::vector<double> & extra_params) {

    if (!width || !height) {
        width = input.size().width;
        height = input.size().height;
        static_display = true;

        if (extra_params.size() >= 2 && extra_params[0] != -1 && extra_params[1] != -1) {
          // extra parameters, for eg from initiator
          min_ws = extra_params[0]*width;
          min_hs = extra_params[1]*height;
            palmbase_x = palmbase_x_new;
            palmbase_y = palmbase_y_new;
        } else {
            min_ws = (4*width)/6;
            min_hs = (4*height)/6;
            palmbase_x = 0.5;
            palmbase_y = 0.9;
        }
        std::cerr << "palm x:" << palmbase_x << " y:" << palmbase_y << "\n";

    }


    {
        ws = min_ws;
        hs = min_hs;

        dx = (ws - 4*gap)/3;
        dy = (hs - 4*gap)/3;
        std::cerr << "anchor ws:" << ws << " hs:" << hs << " gap:" << gap << " dx:" << dx << " dy:" << dy << " palmbase_x:" << palmbase_x << " palmbase_y:" << palmbase_y  << "\n";
        std::cerr << " palmbase_x:" << palmbase_x << " palmbase_y:" << palmbase_y  << "\n";

        xs[0] = (palmbase_x*width) - (ws/2);
        xs[1] = xs[0]+gap;
        xs[2] = xs[1]+dx+gap;
        xs[3] = xs[2]+dx+gap;

        ys[0] = (palmbase_y*height) - hs;
        ys[1] = ys[0]+gap;
        ys[2] = ys[1]+dy+gap;
        ys[3] = ys[2]+dy+gap;
    }

    
    std::cerr << "anchor x0:" << xs[0] << " y0:" << ys[0] << " x1:" << xs[1] << " y1:" << ys[1] << " x2:" << xs[2] << " y2:" << ys[2] << " x3:" << xs[3] << " y3:" << ys[3] << "\n";
    
    if (pointer_x != -1 && pointer_y != -1) {
        selected_i_prv = selected_i;
        selected_j_prv = selected_j;

        selected_i = -1;
        selected_j = -1;

        for (int i = 1;i <= 3; i ++) {
          if (pointer_x > xs[i] && pointer_x < xs[i] + dx) {
            selected_i = i;
            break;
          }     
        }

        for (int j = 1;j <= 3; j ++) {
          if (pointer_y > ys[j] && pointer_y < ys[j] + dy) {
            selected_j = j;
            break;
          }
        }

        if (selected_i == -1 || selected_j == -1) {
            selected_i = -1;
            selected_j = -1;
        }

        if (selected_i != -1) {
            if (selected_i != selected_i_prv || selected_j != selected_j_prv) {
                message = "Picked ";
                message += std::to_string((selected_j-1)*3 + selected_i);
            }
        } else {
            selected_i = selected_i_prv;
            selected_j = selected_j_prv;
        }
    }


    cv::Mat overlay;
    input.copyTo(overlay);

    cv::rectangle(
        overlay, 
        cv::Point(xs[0], ys[0]), cv::Point(xs[0]+ws, ys[0]+hs), 
        cv::Scalar(25, 25, 125),
        -1, 
        cv::LINE_8,
        0);

    for (int i = 1; i <= 3; i ++ ) {
        for (int j = 1; j <= 3; j ++) {
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

    if (selected_i != -1) {
        cv::putText(overlay, //target image
            message, //text
            cv::Point(width/3, height - 20), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            1.0,
            CV_RGB(240, 240, 240), //font color
            2);
    }

    return overlay;
}

void AnchorStatic::reset_palmbase() {
    // palmbase_x = 0;
    // palmbase_y = 0;
    // ws = 0;
    // hs = 0;
}