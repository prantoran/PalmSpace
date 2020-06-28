#ifndef INITIATORS_H
#define INITIATORS_H

#include <vector>
#include <string>
#include <iostream>
#include <tuple>

class Initiator {
    public:
    std::string name;
    std::vector<double> extra_params;
    bool show_display;

    virtual ~Initiator();

    // warning: changes the actual arguments
    virtual bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points) = 0;
    
    virtual std::vector<double> & params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points) = 0;

};

class InitiatorDefault: public Initiator {
    public:
    std::vector<std::vector<std::tuple<double, double, double>>> pointsConvex;
    double areas[2];
    InitiatorDefault();
    bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points);
    std::vector<double> & params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points);
};

class InitiatorTwoHand: public Initiator {
    double topleft_x, topleft_y, bottomright_x, bottomright_y;
    double raw_width, raw_height;
    public:
    std::vector<std::vector<std::tuple<double, double, double>>> pointsConvex;
    double areas[2];
    InitiatorTwoHand();
    bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points);
    std::vector<double> & params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points);
};


void convex_hull(std::vector<std::tuple<double, double, double>>& a);

double area(const std::vector<std::tuple<double, double, double>>& fig);

#endif