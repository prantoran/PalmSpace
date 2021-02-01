#ifndef INITIATORS_H
#define INITIATORS_H

#include <vector>
#include <string>
#include <iostream>
#include <tuple>

#include "desktop/config/config.h"


const int REF_IDX1 = 2; 
const int REF_IDX2 = 5; 
const int INDEXTOP_IDX = 8;
const int MIDDLEFINGERBASE = 9;
const int PALMBASE_IDX = 0;
const int INDEXBASE_IDX = 17; // adjusted for left hand

const double MIN_WIDTH = 0.1;
const double MIN_HEIGHT = 0.1;

const double AREA_THRESHOLD = 0.06;
const double SMALLAREA_THRESHOLD = 0.008;


class Initiator {
    public:
    std::string name;
    bool show_display;
    bool strict;
    
    std::vector<std::vector<std::tuple<double, double, double>>> pointsConvex;
    double areas[2];

    bool valid[2];
    
    virtual ~Initiator();

    // warning: changes the actual arguments
    virtual bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params) = 0;
    virtual void params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params) = 0;
};

class InitiatorDefault: public Initiator {
    public:
    
    InitiatorDefault();
    ~InitiatorDefault();

    bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);
    void params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);
};

class InitiatorTwoHand: public Initiator {
    double topleft_x, topleft_y, bottomright_x, bottomright_y;
    double raw_width, raw_height, raw_width_half;
    double palmbase_x, palmbase_y;

    public:
    
    InitiatorTwoHand();
    ~InitiatorTwoHand();

    bool inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);
    void params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params);
};


void convex_hull(std::vector<std::tuple<double, double, double>>& a);

double area(const std::vector<std::tuple<double, double, double>>& fig);

#endif