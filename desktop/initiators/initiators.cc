#include "initiators.h"

Initiator::~Initiator() {
    for (int i = 0; i < pointsConvex.size(); i ++) {
        pointsConvex[i].clear();
    }
    pointsConvex.clear();
}

Initiator::Initiator() {
    m_debug = false;
}