#include "initiators.h"

Initiator::~Initiator() {
    std::cerr << "destroying Initiator abstract class:" + name + "\n";
}