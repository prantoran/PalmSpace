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
        std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params) {
    

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
  Parameters & params) {

    params.set_total_hands(points.size());

    params.set_raw_dimensions(raw_width, raw_height);

    params.set_palmbase(
        std::make_tuple(palmbase_x, palmbase_y, 0));
    
    params.m_cursor_id = -1;
  
    int idx = 0;

    // TODO refactor common code block below
    if (points[idx].size() > INDEXTOP_IDX) {
        params.m_cursor_id = idx;
        params.set_primary_cursor(points[idx][INDEXTOP_IDX]);
    } else {
        params.m_primary_cursor.reset();
    }

    if (points[idx].size() > MIDDLEFINGERBASE) {
        params.m_cursor_middlebase_id = idx;      
        params.set_primary_cursor_middlefinger_base(points[idx][MIDDLEFINGERBASE]);
    } else {
        params.m_primary_cursor_middlefinger_base.reset();
    }
}