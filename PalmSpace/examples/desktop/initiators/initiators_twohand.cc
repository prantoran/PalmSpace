#include "initiators.h"

const int REF_IDX1 = 2; 
const int REF_IDX2 = 5; 
const int INDEXTOP_IDX = 8;

const double MIN_WIDTH = 0.1;
const double MIN_HEIGHT = 0.1;

const double AREA_THRESHOLD = 0.06;



InitiatorTwoHand::InitiatorTwoHand() {
    name = "Initiator_TwoHand";
    topleft_x = -1;
    topleft_y = -1;
    bottomright_x = -1;
    bottomright_y = -1;

    pointsConvex.resize(2);
}


bool InitiatorTwoHand::inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points) {
    

    for (int i = 0; i < 2; i ++) {
        pointsConvex[i].clear();
        for (auto & u: points[i]) {
            pointsConvex[i].emplace_back(u);
        }
    }

    for (int i = 0; i < 2; i ++) {
      areas[i] = 0;
      if (points[i].size() > 2) {
        convex_hull(pointsConvex[i]);
        areas[i] = area(pointsConvex[i]);
      }
    }

    if (areas[0] < areas[1]) {
      std::swap(points[0], points[1]);
      std::swap(areas[0], areas[1]);
    }
    

    // std::cerr <<"inspect area0:" << areas[0] << " area1:" << areas[1] << "\n";


    if ((areas[0]+areas[1]) > AREA_THRESHOLD && topleft_x == -1) {
        if (points[0].size() > REF_IDX2 && points[1].size() > REF_IDX2) {
            topleft_x = (std::get<0>(points[0][REF_IDX1]) + std::get<0>(points[0][REF_IDX2]))/2;
            topleft_y = (std::get<1>(points[0][REF_IDX1]) + std::get<1>(points[0][REF_IDX2]))/2;

            bottomright_x = (std::get<0>(points[1][REF_IDX1]) + std::get<0>(points[1][REF_IDX2]))/2;
            bottomright_y = (std::get<1>(points[1][REF_IDX1]) + std::get<1>(points[1][REF_IDX2]))/2;

            if (topleft_x > bottomright_x) {
                topleft_x += bottomright_x;
                bottomright_x = topleft_x-bottomright_x;
                topleft_x -= bottomright_x;
            }

            if (topleft_y > bottomright_y) {
                topleft_y += bottomright_y;
                bottomright_y = topleft_y-bottomright_y;
                topleft_y -= bottomright_y;
            }

            raw_width = bottomright_x-topleft_x;
            raw_height = bottomright_y-topleft_y;

            // std::cerr << "topleft_x:" << topleft_x << " bottomright_x:" << bottomright_x << " raw_width:" << raw_width << "\n";

            if (raw_width < MIN_WIDTH || raw_height < MIN_HEIGHT) {
                topleft_x = -1;
                topleft_y = -1;
                bottomright_x = -1;
                bottomright_y = -1;
            }
        }
    }



    show_display = false;

    if (topleft_x != -1) {
        show_display = true;
    }

    std::cerr << "done setting show_display\n";

    return show_display;
}

std::vector<double> & InitiatorTwoHand::params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points) {


    // params() to be called after inspect() returns true

    extra_params.resize(6, -1);
     /*
      0: min_ws_ratio wrt width
      1: min_hs_ratio wrt height
      2: palmbase_x
      3: palmbase_y
      4: otherindex_x
      5: otherindex_y
    */

    extra_params[0] = raw_width;
    extra_params[1] = raw_height;

    extra_params[2] = topleft_x + (raw_width / 2);
    extra_params[3] = bottomright_y;

    if (points[0].size() > INDEXTOP_IDX) {
        extra_params[4] = std::get<0>(points[0][INDEXTOP_IDX]);
        extra_params[5] = std::get<1>(points[0][INDEXTOP_IDX]);
    }

    return extra_params;
}