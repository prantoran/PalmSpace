#include "initiators.h"

Initiator::~Initiator() {
    extra_params.clear();

    for (int i = 0; i < pointsConvex.size(); i ++) {
        pointsConvex[i].clear();
    }
    pointsConvex.clear();

    std::cerr << "destroying Initiator abstract class:" + name + "\n";
}