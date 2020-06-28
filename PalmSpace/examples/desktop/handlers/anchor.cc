

#include "mediapipe/framework/port/opencv_imgproc_inc.h"

#include "PalmSpace/examples/desktop/anchors/anchors.h"
#include "handlers.h"


cv::Mat AnchorHandler::transform(
        const cv::Mat& input, 
        double palmbase_x, double palmbase_y, 
        double area, 
        int pointer_x, int pointer_y,
        const std::vector<double> & extra_params) {

    if (_choice == 1) {
        return _dynamic.transform(input, palmbase_x, palmbase_y, area, pointer_x, pointer_y, extra_params);
    } else {
        return _static.transform(input, palmbase_x, palmbase_y, area, pointer_x, pointer_y, extra_params);
    }
}

void AnchorHandler::reset_palmbase() {
    if (_choice == 1) {
        _dynamic.reset_palmbase();
    } else {
        _static.reset_palmbase();
    }
}

std::tuple<int, int> AnchorHandler::selectedIndexes() {
    if (_choice == 1) {
        return _dynamic.selectedIndexes();
    } else {
        return _static.selectedIndexes();
    }
}

void AnchorHandler::setGreen(int i, int j) {
    if (_choice == 1) {
        _dynamic.setGreen(i, j);
    } else {
        _static.setGreen(i, j);
    }
}

bool AnchorHandler::static_display() {
    if (_choice == 1) {
        return _dynamic.static_display;
    }

    return _static.static_display;
}