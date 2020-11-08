#include "desktop/initiators/initiators.h"
#include "handlers.h"

bool InitiatorHandler::inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points) {
    
    if (_choice == 1) {
        return _default.inspect(points);
    } else if (_choice == 2) {
        return _twohand.inspect(points);
    }

    return _default.inspect(points);
}


void InitiatorHandler::params(
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    ExtraParameters & parameters) {
    
    switch (_choice) {
        case 1:
        _default.params(points, parameters);
        break;
        case 2:
        _twohand.params(points, parameters);
        break;
        default:
        _default.params(points, parameters);
    }
}


void InitiatorHandler::setStrict(bool _strict) {
    if (_choice == 1) {
        _default.strict = _strict;
    } else if (_choice == 2) {
        _twohand.strict = _strict;
    }
}