#include "handlers.h"
#include "desktop/triggers/triggers.h"

#include <stdarg.h>


void TriggerHandler::update(
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    ExtraParameters & params) {
    
    switch (_choice) {
        case 1:
            _thumb.update(input_image, points, params);
        case 2:
            _thumb_other.update(input_image, points, params);
        case 3:
            _pinch.update(input_image, points, params);
        case 4:
            _wait.update(input_image, points, params);
        case 5:
            _tappalm.update(input_image, points, params);
        case 6:
            _dwell.update(input_image, points, params);
        case 7:
            _tap_depth_area.update(input_image, points, params);
        default:
            std::cout << "handlers.trigger: invalid trigger choice\n";
            _dwell.update(input_image, points, params);
    }
}

TRIGGER::state TriggerHandler::status() {   
    switch (_choice) {
        case 1:
            return _thumb.status();
        case 2:
            return _thumb_other.status();
        case 3:
            return _pinch.status();
        case 4:
            return _wait.status();
        case 5:
            return _tappalm.status();
        case 6:
            return _dwell.status();
        case 7:
            return _tap_depth_area.status();
        default:
            return TRIGGER::INVALID;
    }
}

