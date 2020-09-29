// #include "desktop/initiators/initiators.h"
#include "config.h"


ExtraParameters::ExtraParameters() {
    extra_params = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    psize = extra_params.size();
}


void ExtraParameters::set(int i, double v) {
    extra_params[i] = v;
}


void ExtraParameters::set(const std::vector<double> & p) {
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
    return extra_params[4] != -1;
}


void ExtraParameters::set_palmbase(const std::tuple<double, double, double> & p) {
    extra_params[2] = std::get<0>(p);
    extra_params[3] = std::get<1>(p);
}


void ExtraParameters::set_palmbase(double x, double y) {
    extra_params[2] = x;
    extra_params[3] = y;
}


void ExtraParameters::get_palmbase(double &x, double &y) {
    x = extra_params[2];
    y = extra_params[3];
}

