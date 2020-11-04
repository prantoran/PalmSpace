

#include "mediapipe/framework/port/opencv_imgproc_inc.h"

#include "desktop/anchors/anchors.h"
#include "handlers.h"


void AnchorHandler::calculate(
        const cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase, 
        double scale_ratio, 
        int pointer_x, int pointer_y,
        ExtraParameters & params) {

    switch (_choice) {
        case 1:
            _dynamic.calculate(
                input, 
                palmbase, 
                indexbase, 
                scale_ratio, 
                pointer_x, pointer_y, 
                params);
            return;
        case 2:
            _static.calculate(
                input, 
                palmbase, 
                indexbase, 
                scale_ratio, 
                pointer_x, pointer_y, 
                params);
            return;
        case 3:
            _midair.calculate(
                input, 
                palmbase, 
                indexbase, 
                scale_ratio, 
                pointer_x, pointer_y, 
                params);
            return;
        default:
            std::cout << "ERROR handlers/anchor.cc calculate() invalid anchor choice\n";
            return;
    }
}


void AnchorHandler::draw(
        cv::Mat& input, 
        const std::tuple<double, double, double> & palmbase,
        const std::tuple<double, double, double> & indexbase, 
        double scale_ratio, 
        int pointer_x, int pointer_y,
        const ExtraParameters & params) {


    switch (_choice) {
        case 1:
            _dynamic.draw(
                input, 
                palmbase, 
                indexbase, 
                scale_ratio, 
                pointer_x, pointer_y, 
                params);
            return;
        case 2:
            _static.draw(
                input, 
                palmbase, 
                indexbase, 
                scale_ratio, 
                pointer_x, pointer_y, 
                params);
            return;
        case 3:
            _midair.draw(
                input, 
                palmbase, 
                indexbase, 
                scale_ratio, 
                pointer_x, pointer_y, 
                params);
            return;
        default:
            std::cout << "ERROR handlers/anchor.cc draw() invalid anchor choice\n";
            return;
    }
}


void AnchorHandler::reset_palmbase() {
    switch (_choice) {
        case 1:
            _dynamic.reset_palmbase();
            return;
        case 2:
            _static.reset_palmbase();
            return;
        case 3:
            _midair.reset_palmbase();
            return;
        default:
            std::cout << "ERROR handlers/anchor.cc reset_palmbase() invalid anchor choice\n";
            return;
    }
}


void AnchorHandler::reset_indexbase() {
    switch (_choice) {
        case 1:
            _dynamic.reset_indexbase();
            return;
        case 2:
            _static.reset_indexbase();
            return;
        case 3:
            _midair.reset_indexbase();
            return;
        default:
            std::cout << "ERROR handlers/anchor.cc reset_indexbase() invalid anchor choice\n";
            return;
    }
}


std::tuple<int, int> AnchorHandler::selectedIndexes() {
    switch (_choice) {
        case 1:
            return _dynamic.selectedIndexes();
        case 2:
            return _static.selectedIndexes();
        case 3:
            return _midair.selectedIndexes();
        default:
            std::cout << "ERROR handlers/anchor.cc selectedIndexes() invalid anchor choice\n";
            return std::make_tuple(-1, -1);
    }
}


void AnchorHandler::highlightSelected() {
    switch (_choice) {
        case 1:
            _dynamic.highlightSelected();
            return;
        case 2:
            _static.highlightSelected();
            return;
        case 3:
            _midair.highlightSelected();
            return;
        default:
            std::cout << "ERROR handlers/anchor.cc highlightSelected() invalid anchor choice\n";
            return;
    }
}


bool AnchorHandler::static_display() {
    switch (_choice) {
        case 1:
            return _dynamic.static_display;
        case 2:
            return _static.static_display;
        case 3:
            return _midair.static_display;
        default:
            std::cout << "ERROR handlers/anchor.cc static_display() invalid anchor choice\n";
            return false;
    }       
}


void AnchorHandler::setDivisions(int _divisions) {
    switch (_choice) {
        case 1:
            _dynamic.setDivisions(_divisions);
            return;
        case 2:
            _static.setDivisions(_divisions);
            return;
        case 3:
            _midair.setDivisions(_divisions);
            return;
        default:
            std::cout << "ERROR handlers/anchor.cc setDivisions() invalid anchor choice\n";
            return;
    }
}


int AnchorHandler::getDivisions() {
    switch (_choice) {
        case 1:
            return _dynamic.getDivisions();
        case 2:
            return _static.getDivisions();
        case 3:
            return _midair.getDivisions();
        default:
            std::cout << "ERROR handlers/anchor.cc getDivisions() invalid anchor choice\n";
            return -1;
    }
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
            std::cout << "ERROR handlers/anchor.cc getGrid() invalid anchor choice\n";
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
            std::cout << "ERROR handlers/anchor.cc getGridTopLeft() invalid anchor choice\n";
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
            std::cout << "ERROR handlers/anchor.cc getGridBottomRight() invalid anchor choice\n";
            return _dynamic.getGridBottomRight();
    }
};

void AnchorHandler::setScreenSize(eScreenSize size) {
    switch (_choice) {
        case 1:
            _dynamic.setScreenSize(size);
            break;
        case 2:
            _static.setScreenSize(size);
            break;
        case 3:
            _midair.setScreenSize(size);
            break;
        default:
            std::cout << "ERROR handlers/anchor.cc setScreenSize() invalid anchor choice\n";
            return;
    }
}