#include "triggers.h"

const int index_top = 8;
const int CACHE_SIZE = 9;


TriggerTapPalm::TriggerTapPalm() {
    cnt = 0;
    cur_state = TRIGGER::OPEN;
    for (int i = 0; i < 9; i ++) {
        cache[i] = 0;
    }

    cache_id = 0;
}

TriggerTapPalm::TriggerTapPalm(int _width, int _height) {
    width = _width;
    height = _height;
    cur_state = TRIGGER::OPEN;
    cnt = 0;
    prev_zvalue = 0;
    prev_handID = 0;
    hand_switch_ignore_cnt = 0;

    for (int i = 0; i < 9; i ++) {
        cache[i] = 0;
    }

    cache_id = 0;
}

void TriggerTapPalm::update(
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    std::vector<double> & extra_params) {

    if (points.size() < 0) {
        std::cerr << "trigger_tap_palm: less than two hands, resetting trigger\n";
        return;
    }

    if (points.size() == 1) {
        hand_ID = 0;
        std::cerr << "trigger_tap_palm: using first hand\n";
    } {
        hand_ID = 0;
        std::cerr << "trigger_tap_palm: using second hand\n";
    }

    if (points[hand_ID].size() < 9) {
        std::cerr << "trigger_tap_palm=not enough points for free palm\n";
        return;
    }

    std::cout << "trigger_tap_palm extra_params[6]:" << extra_params[6] << "\n";
    if (extra_params[4] > 0) { // other hand index x axis is set (0-1]
        zvalue = 0.5*prev_zvalue + 0.5*extra_params[6]*10000;
        std::cout << "trigger_tap_palm getting from extra params\n";
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

        zvalue = 0.5*prev_zvalue + 0.5*std::get<2>(points[hand_ID][index_top])*10000;
    }

    prev_zvalue = zvalue;

    cache[cache_id] = zvalue;
    cache_id = (cache_id+1) % CACHE_SIZE;
    // if (zvalue < -1500) {
    //     cnt ++;
    //     if (cnt > 15) {
    //         cnt = 0;
    //         if (cur_state == TRIGGER::RELEASED) {
    //             cur_state = TRIGGER::OPEN;
    //         }
    //     } else if (cnt > 10) { 
    //         if (cur_state == TRIGGER::OPEN) {
    //             cnt = 0;
    //         } else if (cur_state == TRIGGER::PRESSED) {
    //             cur_state = TRIGGER::RELEASED;
    //         }
    //     }
    // } else if (-1500 < zvalue && zvalue < 0) {
    //     cnt ++;
    //     if (cnt > 5) {
    //         if (cur_state == TRIGGER::OPEN) {
    //             cur_state = TRIGGER::PRESSED;
    //         }
    //     }
    // } else if (zvalue > 0) {
    //     cnt = 0;
    //     cur_state = TRIGGER::OPEN;
    // }

    nd = cache[(cache_id-1+CACHE_SIZE)%CACHE_SIZE];
    mid = cache[(cache_id-(CACHE_SIZE/2) + CACHE_SIZE)%CACHE_SIZE];
    st = cache[cache_id];

    double diff1 = mid - st;
    double diff2 = nd - mid;

    if (diff1 > 100 && diff2 < -100) {
        cur_state = TRIGGER::RELEASED;
    } else {
        cur_state = TRIGGER::OPEN;
    }

    for (int i = 0; i < CACHE_SIZE; i ++) {
        std::cout << cache[i] << " ";
    } std::cout << "\n";
    std::cout << "diff1:" << diff1 << " diff2:" << diff2 << "\n";
    std::cerr << "tap zvalue:" << zvalue << " zindex:" << std::get<2>(points[hand_ID][index_top]) << " zpalm:" << std::get<2>(points[hand_ID][0])<< "\tcnt:" << cnt << "\tcurstate:" << cur_state << "\n";
}

TRIGGER::state TriggerTapPalm::status() {
    return cur_state;
}