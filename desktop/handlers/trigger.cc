#include "handlers.h"
#include "desktop/triggers/triggers.h"

#include <stdarg.h>


void TriggerHandler::update(const std::vector<std::vector<std::tuple<double, double, double>>> & points) {
    if (_choice == 1) {
        _thumb.update(points);
    } else if (_choice == 2) {
        _thumb_other.update(points);
    } else if (_choice == 3) {
        _pinch.update(points);
    } else if (_choice == 4) {
        _wait.update(points);
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
    }
    return TRIGGER::INVALID;
}

