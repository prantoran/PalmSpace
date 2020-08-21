#include "handlers.h"
#include "desktop/triggers/triggers.h"

#include <stdarg.h>


void TriggerHandler::update(
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    std::vector<double> & extra_params) {
    
    if (_choice == 1) {
        _thumb.update(points, extra_params);
    } else if (_choice == 2) {
        _thumb_other.update(points, extra_params);
    } else if (_choice == 3) {
        _pinch.update(points, extra_params);
    } else if (_choice == 4) {
        _wait.update(points, extra_params);
    } else if (_choice == 5) {
        // _tap.update(points);
        _tappalm.update(points, extra_params);
    } else if (_choice == 6) {
        _dwell.update(points, extra_params);
    }
}

TRIGGER::state TriggerHandler::status() {   
    if (_choice == 1) {
        return _thumb.status();
    } else if (_choice == 2) {
        return _thumb_other.status();
    } else if (_choice == 3) {
        return _pinch.status();
    } else if (_choice == 4) {
        return _wait.status();
    } else if (_choice == 5) {
        // return _tap.status();
        return _tappalm.status();
    } else if (_choice == 6) {
        return _dwell.status();
    }
    
    return TRIGGER::INVALID;
}

