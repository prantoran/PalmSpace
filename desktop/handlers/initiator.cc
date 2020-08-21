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


void InitiatorHandler::setStrict(bool _strict) {
    // std::cerr << "initiator_handler setstrict _strict:" << _strict << " init_hcoice:" << _choice << "\n";
    if (_choice == 1) {
        _default.strict = _strict;
    } else if (_choice == 2) {
        _twohand.strict = _strict;
    }
}