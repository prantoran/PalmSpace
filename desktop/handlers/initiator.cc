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


std::vector<double> & InitiatorHandler::params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points) {
    
    if (_choice == 1) {
        return _default.params(points);
    } else if (_choice == 2) {
        return _twohand.params(points);
    }

    return _default.params(points);
}