
#include "triggers.h"
#include <iostream>

// internal linkage, not visible outside
// for external linkage, use extern
const int index_top = 8;

TriggerWait::TriggerWait() {}

TriggerWait::TriggerWait(int _width, int _height, int anchor_choice) {
    width = _width;
    height = _height;

    min_ws = (2*width)/6;
    min_hs = (2*height)/6;

    choice = anchor_choice;
    palmbase_momentum = 0.9;
    gap = 15;

    selected_i = -1;
    selected_j = -1;

    ctime = cur_time();

    for (int i = 0; i < 3; i ++) {
        for (int j = 0; j < 3; j ++) {
            _timestamp[i][j] = ctime;
        }
    }

    cur_state = TRIGGER::OPEN;

    // std::cerr <<"TriggerWait constructor _width:" << _width << " _height:" << _height << " width:" << width << " height:" << height << "\n";
}

void TriggerWait::update(
    const std::vector<std::vector<std::tuple<double, double, double>>> & points,
    std::vector<double> & extra_params) {

    ctime = cur_time();

    pointer_x = (int)std::get<0>(points[2][0]);
    pointer_y = (int)std::get<1>(points[2][0]);

    {
        // ws = (1-palmbase_momentum)*ws + palmbase_momentum*(2*area*width);
        // hs = (1-palmbase_momentum)*hs + palmbase_momentum*(2*area*height);
        // if (ws < min_ws) ws = min_ws;
        // if (hs < min_hs) hs = min_hs;
        ws = min_ws;
        hs = min_hs;

        dx = (ws - 4*gap)/3;
        dy = (hs - 4*gap)/3;

        palmbase_x_prv = palmbase_x;
        palmbase_y_prv = palmbase_y;

        palmbase_x_new = std::get<0>(points[0][0]);
        palmbase_y_new = std::get<1>(points[0][0]);

        palmbase_x = palmbase_x_new;
        palmbase_y = palmbase_y_new;

        if (palmbase_x_prv) {
          palmbase_x = (1-palmbase_momentum)*palmbase_x + palmbase_momentum*palmbase_x_prv;
        }
        
        if (palmbase_y_prv) {
          palmbase_y = (1-palmbase_momentum)*palmbase_y + palmbase_momentum*palmbase_y_prv;
        }
        // std::cerr << "anchor ws:" << ws << " hs:" << hs << " gap:" << gap << " dx:" << dx << " dy:" << dy << " palmbase_x:" << palmbase_x << " palmbase_y:" << palmbase_y << " palmbase_x_prv:" << palmbase_x_prv << " palmbase_y_prv:" << palmbase_y_prv << "\n";

        xs[0] = (palmbase_x*width) - (ws/2);
        xs[1] = xs[0]+gap;
        xs[2] = xs[1]+dx+gap;
        xs[3] = xs[2]+dx+gap;

        ys[0] = (palmbase_y*height) - hs;
        ys[1] = ys[0]+gap;
        ys[2] = ys[1]+dy+gap;
        ys[3] = ys[2]+dy+gap;
    }

    if (pointer_x != -1 && pointer_y != -1) {
        selected_i_prv = selected_i;
        selected_j_prv = selected_j;

        selected_i = -1;
        selected_j = -1;

        for (int i = 1;i <= 3; i ++) {
          if (pointer_x + 3 > xs[i] && pointer_x < 3 + xs[i] + dx) {
            selected_i = i;
            break;
          }     
        }

        for (int j = 1;j <= 3; j ++) {
          if (pointer_y + 3 > ys[j] && pointer_y < 3 + ys[j] + dy) {
            selected_j = j;
            break;
          }
        }

        if (selected_i == -1 || selected_j == -1) {
            selected_i = -1;
            selected_j = -1;
        }
    } else {
        selected_i = -1;
        selected_j = -1;
    }


    std::cerr << "wait x0:" << xs[0] << " y0:" << ys[0] << " x1:" << xs[1] << " y1:" << ys[1] << " x2:" << xs[2] << " y2:" << ys[2] << " x3:" << xs[3] << " y3:" << ys[3] << "\n";
    std::cerr << "wait palmbase_x:" << palmbase_x << " width:" << width << " ws:" << ws << "\n";
    std::cerr  << "wait pointer_x:" << pointer_x << " pointer_y:" << pointer_y << " selected_i: " << selected_i << " selected_j: " << selected_j << " selected_i_prv:" << selected_i_prv << " selected_j_prv:" << selected_j_prv << "\n";


    if (selected_i != -1) {
        if (selected_i_prv != -1) {
            if (selected_i_prv == selected_i && selected_j_prv == selected_j) {
                ptime = _timestamp[selected_i_prv][selected_j_prv];
                // trigger if time diff larger than threshold
                auto d = std::chrono::duration_cast<std::chrono::milliseconds>(ctime - ptime).count();
                std::cerr << "duration:" << d  << "\n";
                if (d > 2000) {
                    cur_state = TRIGGER::RELEASED;
                }
            } else {

                _timestamp[selected_i][selected_j] = ctime;
                _timestamp[selected_i_prv][selected_j_prv] = ctime;

                cur_state = TRIGGER::OPEN;
            }
        } else {
            _timestamp[selected_i][selected_j] = ctime;
            cur_state = TRIGGER::OPEN;
        }
    } else {
        if (selected_i_prv != -1) {
            _timestamp[selected_i_prv][selected_j_prv] = ctime;
        } else {
            // do nothing
        }

        cur_state = TRIGGER::OPEN;
    }

}

TRIGGER::state TriggerWait::status() {
    return cur_state;
}