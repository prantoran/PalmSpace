

#include "mediapipe/framework/port/opencv_imgproc_inc.h"

#include "desktop/anchors/anchors.h"
#include "handlers.h"

void AnchorHandler::calculate(
        const cv::Mat& input, 
        double palmbase_x_new, double palmbase_y_new, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params) {

    if (_choice == 1) {
        _dynamic.calculate(input, palmbase_x_new, palmbase_y_new, interface_scaling_factor, pointer_x, pointer_y, extra_params);
    } else if (_choice == 2) {
        _static.calculate(input, palmbase_x_new, palmbase_y_new, interface_scaling_factor, pointer_x, pointer_y, extra_params);
    } else if (_choice == 3) {
        _midair.calculate(input, palmbase_x_new, palmbase_y_new, interface_scaling_factor, pointer_x, pointer_y, extra_params);
    } else {
        std::cerr << "handler_anchor invalid choice:" << _choice << "\n";
    }
}

void AnchorHandler::draw(
        cv::Mat& input, 
        double palmbase_x_new, double palmbase_y_new, 
        double interface_scaling_factor, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params) {

    if (_choice == 1) {
        _dynamic.draw(input, palmbase_x_new, palmbase_y_new, interface_scaling_factor, pointer_x, pointer_y, extra_params);
    } else if (_choice == 2){
        _static.draw(input, palmbase_x_new, palmbase_y_new, interface_scaling_factor, pointer_x, pointer_y, extra_params);
    } else if (_choice == 3) {
        _midair.draw(input, palmbase_x_new, palmbase_y_new, interface_scaling_factor, pointer_x, pointer_y, extra_params);
    }
}

void AnchorHandler::reset_palmbase() {
    if (_choice == 1) {
        _dynamic.reset_palmbase();
    } else if (_choice == 2) {
        _static.reset_palmbase();
    } else if (_choice == 3) {
        _midair.reset_palmbase();
    }
}

std::tuple<int, int> AnchorHandler::selectedIndexes() {
    if (_choice == 1) {
        return _dynamic.selectedIndexes();
    } else if (_choice == 2) {
        return _static.selectedIndexes();
    } else {
        return _midair.selectedIndexes();
    }
}

void AnchorHandler::highlightSelected() {
    if (_choice == 1) {
        _dynamic.highlightSelected();
    } else if (_choice == 2) {
        _static.highlightSelected();
    } else if (_choice == 3) {
        _midair.highlightSelected();
    }
}

bool AnchorHandler::static_display() {
    if (_choice == 1) {
        return _dynamic.static_display;
    } else if (_choice == 2) {
        return _static.static_display;
    } else {
        return _midair.static_display;
    }
}

void AnchorHandler::setDivisions(int _divisions) {
    if (_choice == 1) {
        _dynamic.setDivisions(_divisions);
    } else if (_choice == 2) {
        _static.setDivisions(_divisions);
    } else if (_choice == 3) {
        _midair.setDivisions(_divisions);
    }
}

int AnchorHandler::getDivisions() {
    if (_choice == 1) {
        return _dynamic.getDivisions();
    } else if (_choice == 2) {
        return _static.getDivisions();
    } else if (_choice == 3) {
        return _midair.getDivisions();
    }

    return -1;
}
