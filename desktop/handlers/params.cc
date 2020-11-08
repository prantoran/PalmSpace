// #include "desktop/initiators/initiators.h"
#include "handlers.h"

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