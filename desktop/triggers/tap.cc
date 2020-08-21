#include "triggers.h"
#include <iostream>

const int index_top = 8;


TriggerTap::TriggerTap() {
    cnt = 0;
    cur_state = TRIGGER::OPEN;
}

TriggerTap::TriggerTap(int _width, int _height) {
    width = _width;
    height = _height;
    cur_state = TRIGGER::OPEN;
    cnt = 0;
    prev_zvalue = 0;
    prev_handID = 0;
    hand_switch_ignore_cnt = 0;
}

void TriggerTap::update(
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    std::vector<double> & extra_params) {

    if (points.size() < 0) {
        std::cerr << "trigger_tap: less than two hands, resetting trigger\n";
        return;
    }

    if (points.size() == 1) {
        hand_ID = 0;
        std::cerr << "trigger_tap: using first hand\n";
    } {
        hand_ID = 0;
        std::cerr << "trigger_tap: using second hand\n";
    }

    if (points[hand_ID].size() < 9) {
        std::cerr << "trigger_tap=not enough points for free palm\n";
        return;
    }

    
    if (extra_params[4] > 0) { // other hand index x axis is set (0-1]
        zvalue = 0.5*prev_zvalue + 0.5*extra_params[6]*100;
        std::cout << "trigger_tap getting from extra params\n";
    } else {
        if (prev_handID != hand_ID) {
            hand_switch_ignore_cnt ++;
            if (hand_switch_ignore_cnt > 2) {
                hand_switch_ignore_cnt = 0;
                prev_handID = hand_ID;
            } else {
                return;
            }
        }

        zvalue = 0.5*prev_zvalue + 0.5*std::get<2>(points[hand_ID][index_top])*100;
    }

    prev_zvalue = zvalue;

    if (-20< zvalue && zvalue < 0) {
        cnt ++;
        if (cnt > 15) {
            cnt = 0;
            if (cur_state == TRIGGER::RELEASED) {
                cur_state = TRIGGER::OPEN;
            }
        } else if (cnt > 10) { 
            if (cur_state == TRIGGER::OPEN) {
                cnt = 0;
            } else if (cur_state == TRIGGER::PRESSED) {
                cur_state = TRIGGER::RELEASED;
            }
        }
    } else if (zvalue < -20) {
        cnt ++;
        if (cnt > 5) {
            if (cur_state == TRIGGER::OPEN) {
                cur_state = TRIGGER::PRESSED;
            }
        }
    } else if (zvalue > 0) {
        cnt = 0;
        cur_state = TRIGGER::OPEN;
    }


    std::cerr << "tap zvalue:" << zvalue << "\tcnt:" << cnt << "\tcurstate:" << cur_state << "\n";
}

TRIGGER::state TriggerTap::status() {
    return cur_state;
}