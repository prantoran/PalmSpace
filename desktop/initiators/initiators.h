#ifndef INITIATORS_H
#define INITIATORS_H

#include <vector>
#include <string>
#include <iostream>
#include <tuple>

#include "desktop/config/config.h"

class Initiator {
    public:
    std::string name;
    std::vector<double> extra_params;
    bool show_display;
    bool strict;
    
    std::vector<std::vector<std::tuple<double, double, double>>> pointsConvex;
    double areas[2];
    
    virtual ~Initiator();

    // warning: changes the actual arguments
    virtual bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points) = 0;
    virtual void params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        ExtraParameters & parameters) = 0;
};

class InitiatorDefault: public Initiator {
    public:
    
    InitiatorDefault();
    bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points);
    void params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        ExtraParameters & parameters);
};

class InitiatorTwoHand: public Initiator {
    double topleft_x, topleft_y, bottomright_x, bottomright_y;
    double raw_width, raw_height, raw_width_half;
    double palmbase_x, palmbase_y;

    public:
    
    InitiatorTwoHand();
    bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points);
    void params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        ExtraParameters & parameters);
};


void convex_hull(std::vector<std::tuple<double, double, double>>& a);

double area(const std::vector<std::tuple<double, double, double>>& fig);

#endif