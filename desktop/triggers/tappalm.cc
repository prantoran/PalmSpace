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
    const cv::Mat & input_image,
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    Parameters & params) {

    if (points.size() < 0) {
        std::cout << "trigger_tap_palm: less than two hands, resetting trigger\n";
        return;
    }

    hand_ID = 0;

    if (points[hand_ID].size() < 9) {
        std::cout << "trigger_tap_palm=not enough points for free palm\n";
        return;
    }

    params.get_other_index_z_value(m_zvalue);

    if (params.is_set_primary_cursor()) { // other hand index x axis is set (0-1]
        zvalue = 0.5*prev_zvalue + 0.5*m_zvalue*10000;
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
}
