

#include "mediapipe/framework/port/opencv_imgproc_inc.h"

#include "desktop/anchors/anchors.h"
#include "handlers.h"


void AnchorHandler::calculate(
        const cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase, 
        double scale_ratio, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params) {

    if (_choice == 1) {
        _dynamic.calculate(input, palmbase, indexbase, scale_ratio, pointer_x, pointer_y, extra_params);
    } else if (_choice == 2) {
        _static.calculate(input, palmbase, indexbase, scale_ratio, pointer_x, pointer_y, extra_params);
    } else if (_choice == 3) {
        _midair.calculate(input, palmbase, indexbase, scale_ratio, pointer_x, pointer_y, extra_params);
    } else {
        std::cerr << "handler_anchor invalid choice:" << _choice << "\n";
    }
}


void AnchorHandler::draw(
        cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase, 
        double scale_ratio, 
        int pointer_x, int pointer_y,
        std::vector<double> & extra_params) {

    if (_choice == 1) {
        _dynamic.draw(input, palmbase, indexbase, scale_ratio, pointer_x, pointer_y, extra_params);
    } else if (_choice == 2){
        _static.draw(input, palmbase, indexbase, scale_ratio, pointer_x, pointer_y, extra_params);
    } else if (_choice == 3) {
        _midair.draw(input, palmbase, indexbase, scale_ratio, pointer_x, pointer_y, extra_params);
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


void AnchorHandler::reset_indexbase() {
    if (_choice == 1) {
        _dynamic.reset_indexbase();
    } else if (_choice == 2) {
        _static.reset_indexbase();
    } else if (_choice == 3) {
        _midair.reset_indexbase();
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


cv::Rect AnchorHandler::getGrid() {
    switch(_choice) {
        case 1:
            return _dynamic.getGrid();
        case 2:
            return _static.getGrid();
        case 3:
            return _midair.getGrid();
        default:
            return _dynamic.getGrid();
    }
}


cv::Point AnchorHandler::getGridTopLeft() {
    switch (_choice) {
        case 1:
            return _dynamic.getGridTopLeft();
        case 2:
            return _static.getGridTopLeft();
        case 3:
            return _midair.getGridTopLeft();
        default:
            return _dynamic.getGridTopLeft();
    }
};
cv::Point AnchorHandler::getGridBottomRight() {
    switch (_choice) {
        case 1:
            return _dynamic.getGridBottomRight();
        case 2:
            return _static.getGridBottomRight();
        case 3:
            return _midair.getGridBottomRight();
        default:
            return _dynamic.getGridBottomRight();
    }
};