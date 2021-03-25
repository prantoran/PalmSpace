

#include "trigger.h"

#include <string>

Trigger::Trigger() {
    m_debug = false;
}

Trigger::~Trigger() {}



std::string Trigger::state_str() {
    std::string ret = "invalid";
    if (cur_state == TRIGGER::OPEN) ret = "open";
    if (cur_state == TRIGGER::RELEASED) ret = "released";
    if (cur_state == TRIGGER::PRESSED) ret = "pressed";
    return ret;
}

std::chrono::milliseconds cur_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
}


TRIGGER::state Trigger::status() {
    return cur_state;
}

