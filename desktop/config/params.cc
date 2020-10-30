// #include "desktop/initiators/initiators.h"
#include "config.h"
#include <iostream>

ExtraParameters::~ExtraParameters() {
    std::cout << "extraparameters killed\n";
}


ExtraParameters::ExtraParameters() {
    extra_params = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    psize = extra_params.size();

    reset();
}

ExtraParameters::ExtraParameters(bool _load_video) {
    extra_params = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    psize = extra_params.size();

    reset();
    init(_load_video);
}


void ExtraParameters::reset() {
    for (int i = 0; i < psize; i ++) {
        extra_params[i] = -1;
    }

    indexbase = std::make_tuple(-1, -1, -1);

    load_video = false;
}


void ExtraParameters::init(bool _load_video) {
    load_video = _load_video;
}

void ExtraParameters::set(int i, double v) {
    extra_params[i] = v;
}


void ExtraParameters::set(const std::vector<double> & p) {
    std::cerr << "params reset called\n";
    for (int i = 0; i < psize; i ++) {
        extra_params[i] = p[i];
    }
}


double ExtraParameters::at(int i) {
    return extra_params[i];
}



void ExtraParameters::set_indexfinger(const std::tuple<double, double, double> & p) {
    extra_params[4] = std::get<0>(p);
    extra_params[5] = std::get<1>(p);
    extra_params[6] = std::get<2>(p);
}

void ExtraParameters::get_indexfinger(double &x, double &y) {
    x = extra_params[4];
    y = extra_params[5];
}


bool ExtraParameters::is_set_indexfinger() {
    return extra_params[4] > 0.01;
}


void ExtraParameters::set_palmbase(const std::tuple<double, double, double> & p) {
    palmbase = p;
}


void ExtraParameters::set_palmbase(double x, double y) {
    palmbase = std::make_tuple(x, y, 0);
}


void ExtraParameters::get_palmbase(double &x, double &y) {
    x = std::get<0>(palmbase);
    y = std::get<1>(palmbase);

    // for adjusting topleft pos of grid, since pointing 
    // to top-left grids with other palm's index finger is hard
    x += 0.11;
    y += 0.16;
}


void ExtraParameters::get_palmbase(std::tuple<double, double, double> & p) {
    double x = std::get<0>(palmbase);
    double y = std::get<1>(palmbase);

    // for adjusting topleft pos of grid, since pointing 
    // to top-left grids with other palm's index finger is hard
    x += 0.11;
    y += 0.16;

    p = std::make_tuple(x, y, 0);
}


void ExtraParameters::set_indexbase(const std::tuple<double, double, double> & p) {
    indexbase = p;
}


void ExtraParameters::set_indexbase(double x, double y) {
    indexbase = std::make_tuple(x, y, 0);
}


void ExtraParameters::get_indexbase(double &x, double &y) {
    x = std::get<0>(indexbase);
    y = std::get<1>(indexbase);
}


void ExtraParameters::get_indexbase(std::tuple<double, double, double> & p) {

    double x = std::get<0>(indexbase);
    double y = std::get<1>(indexbase);

    // for adjusting topleft pos of grid, since pointing 
    // to top-left grids with other palm's index finger is hard
    // x += 0.03;
    // y += 0.09;

    p = std::make_tuple(x, y, 0);
}