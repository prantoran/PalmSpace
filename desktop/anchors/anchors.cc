#include "anchors.h"

#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"


Anchor::~Anchor() {
}


void Anchor::setConfig(int _width, int _height) {
    width = _width;
    height = _height;
    progress_maxwidth = width/8;
    progress_maxheight = height/28;
}

std::tuple<int, int> Anchor::selectedIndexes() {
    return std::make_tuple(selected_i, selected_j);
}

void Anchor::highlightSelected() {
    green_i = selected_i;
    green_j = selected_j;

    message_selected = "Selected: " + std::to_string(divisions*(green_j-1) + green_i);
}

void Anchor::setDivisions(int _divisions) {
    divisions = _divisions;
}

int Anchor::getDivisions() {
    return divisions;
}


void Anchor::drawProgressBar(cv::Mat & _image, double _progress) {
    // modifies _image with a progress bar with _progress
    if (_progress >= 0) {

        double pwidth = _progress * (progress_maxwidth);

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


void Anchor::setupGrid(double enlarged_topleft_x, double enlarged_topleft_y) {
    // setup tile coords

    // enlarged means topleft coords are received after multiplying ratios by width and height
    ws = min_ws;
    hs = min_hs;

    dx = (ws - (divisions+1)*gap)/divisions;
    dy = (hs - (divisions+1)*gap)/divisions;

    xs[0] = enlarged_topleft_x;
    xs[1] = xs[0]+gap;
    for (int i = 2; i <= divisions; i ++) {
      xs[i] = xs[i-1]+dx+gap;
    }

    ys[0] = enlarged_topleft_y;
    ys[1] = ys[0]+gap;
    for (int i = 2; i <= divisions; i ++) {
      ys[i] = ys[i-1]+dy+gap;
    }
}


void Anchor::setupSelection(int index_pointer_x, int index_pointer_y) {
    // checks whether index_pointer is within a til in the grid,
    // if yes then setup selected i,j and message

    if (index_pointer_x != -1 && index_pointer_y != -1) {
        selected_i_prv = selected_i;
        selected_j_prv = selected_j;

        selected_i = -1;
        selected_j = -1;

        for (int i = 1;i <= divisions; i ++) {
          if (index_pointer_x > xs[i] && index_pointer_x < xs[i] + dx) {
            selected_i = i;
            break;
          }     
        }

        for (int j = 1;j <= divisions; j ++) {
          if (index_pointer_y > ys[j] && index_pointer_y < ys[j] + dy) {
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
                message = "Highlighted: ";
                message += std::to_string((selected_j-1)*divisions + selected_i);
            }
        } else {
            selected_i = selected_i_prv;
            selected_j = selected_j_prv;
        }
    }
}


void Anchor::drawTextHighlighted(cv::Mat & overlay) {
    if (selected_i != -1) {
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
    ws = 0;
    hs = 0;
}


void Anchor::reset_indexbase() {
    indexbase_x = -1;
    indexbase_y = -1;
    ws = 0;
    hs = 0;
}

void Anchor::reset_grid() {

    int len = (sizeof(xs)/sizeof(*xs));
    for (int i = 0; i < len; i ++) {
        xs[i] = 0;
        ys[i] = 0;
    }
}


cv::Rect Anchor::getGrid() {
    return cv::Rect(cv::Point(xs[0], ys[0]), cv::Point(xs[0]+ws, ys[0]+hs));
}

cv::Point Anchor::getGridTopLeft() {
    return cv::Point(xs[0], ys[0]);
}


cv::Point Anchor::getGridBottomRight() {
    return cv::Point(xs[0]+ws, ys[0]+hs);
}


void Anchor::setScreenSize(eScreenSize size) {
    screen.size = size;
}