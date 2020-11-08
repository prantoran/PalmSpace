#include "initiators.h"

Initiator::~Initiator() {
    extra_params.clear();

    for (int i = 0; i < pointsConvex.size(); i ++) {
        pointsConvex[i].clear();
    }
    pointsConvex.clear();
}