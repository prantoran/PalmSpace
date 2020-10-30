#include "initiators.h"


InitiatorTwoHand::InitiatorTwoHand() {
    name = "Initiator_TwoHand";
    topleft_x = -1;
    topleft_y = -1;
    bottomright_x = -1;
    bottomright_y = -1;

    pointsConvex.resize(2);
    strict = false;
}

InitiatorTwoHand::~InitiatorTwoHand() {
    std::cout << "twohand initiator killed\n";
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
            raw_width_half = raw_width / 2;

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
        palmbase_x = topleft_x + raw_width_half;
        palmbase_y = bottomright_y;
    }

    return show_display;
}


void InitiatorTwoHand::params(
  const std::vector<std::vector<std::tuple<double, double, double>>> & points,
  ExtraParameters & parameters) {

    parameters.set(0, raw_width);
    parameters.set(1, raw_height);

    parameters.set_palmbase(palmbase_x, palmbase_y);

    if (points[0].size() > INDEXTOP_IDX) {
        parameters.set_indexfinger(points[0][INDEXTOP_IDX]);
    }
}