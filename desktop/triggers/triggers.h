#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <tuple>
#include <vector>
#include <chrono>
#include <string>
#include <iostream>

namespace TRIGGER {
    typedef enum{
        OPEN,
        PRESSED, 
        RELEASED,
        INVALID
    } state;
}

std::chrono::milliseconds cur_time();

class Trigger {
    public:
    int width, height;
    TRIGGER::state cur_state;

    std::string state_str(); 

    virtual ~Trigger();
    

    virtual void update(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        std::vector<double> & extra_params) = 0;
    virtual TRIGGER::state status() = 0;
};

class TriggerThumb: public Trigger {
    public:

    TriggerThumb();
    TriggerThumb(int _width, int _height);

    void update(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        std::vector<double> & extra_params);
    TRIGGER::state status();
};


class TriggerThumbOther: public Trigger {
    public:

    TriggerThumbOther();
    TriggerThumbOther(int _width, int _height);

    void update(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        std::vector<double> & extra_params);
    TRIGGER::state status();
};

class TriggerPinch: public Trigger {
    public:

    TriggerPinch();
    TriggerPinch(int _width, int _height);

    void update(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        std::vector<double> & extra_params);
    TRIGGER::state status();
};

class TriggerWait: public Trigger {
    std::chrono::milliseconds _timestamp[3][3], ctime, ptime;
    int min_ws, min_hs, ws, hs;
    int gap, xs[4], ys[4], dx, dy;
    double palmbase_momentum;
    double palmbase_x, palmbase_y, palmbase_x_prv, palmbase_y_prv;
    double palmbase_x_new, palmbase_y_new;

    int pointer_x, pointer_y;
    int selected_i, selected_j, selected_i_prv, selected_j_prv; 
        
    public:

    int choice;
    TriggerWait();
    TriggerWait(int _width, int _height, int _anchor_choice);

    void update(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        std::vector<double> & extra_params);
    TRIGGER::state status();
};

class TriggerTap: public Trigger {
    public:

    int cnt;
    double zvalue, prev_zvalue;
    int hand_ID, prev_handID, hand_switch_ignore_cnt;
    
    TriggerTap();
    TriggerTap(int _width, int _height);

    void update(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        std::vector<double> & extra_params);
    TRIGGER::state status();
};

class TriggerTapPalm: public Trigger {

    public:

    int cnt;
    double zvalue, prev_zvalue;
    int hand_ID, prev_handID, hand_switch_ignore_cnt;
    
    double cache[9];
    int cache_id;

    double st, mid, nd;

    TriggerTapPalm();
    TriggerTapPalm(int _width, int _height);

    void update(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        std::vector<double> & extra_params);
    TRIGGER::state status();
};


class TriggerDwell: public Trigger {
    std::chrono::milliseconds _timestamp[11][11], ctime, ptime;
    int selected_i, selected_j, selected_i_prv, selected_j_prv; 
        
    public:

    TriggerDwell();

    void update(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        std::vector<double> & extra_params);
    TRIGGER::state status();
};

#endif
