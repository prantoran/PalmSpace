

#include "triggers.h"

#include <tuple>

Trigger::~Trigger() {}


std::string Trigger::state_str() {
    if (cur_state == TRIGGER::OPEN) return "open";
    if (cur_state == TRIGGER::RELEASED) return "released";
    if (cur_state == TRIGGER::PRESSED) return "pressed";
    return "invalid";
}