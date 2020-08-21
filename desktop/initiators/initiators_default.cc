#include "initiators.h"


const double AREA_THRESHOLD = 0.06;
const double SMALLAREA_THRESHOLD = 0.01;
const int PALMBASE_IDX = 0;
const int INDEXTOP_IDX = 8;




InitiatorDefault::InitiatorDefault() {
    name = "Initiator_Default";
    pointsConvex.resize(2);
    strict = false;
}

bool InitiatorDefault::inspect(
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

    show_display = false;
    if (areas[0] > AREA_THRESHOLD) {
        show_display = true;
    }

    return show_display;
}

std::vector<double> & InitiatorDefault::params(
        const std::vector<std::vector<std::tuple<double, double, double>>> & points) {
    
    // params() to be called after inspect() returns true

    extra_params.resize(9, -1);
    /*
      0: min_ws_ratio wrt width
      1: min_hs_ratio wrt height
      2: palmbase_x
      3: palmbase_y
      4: otherindex_x
      5: otherindex_y
      6: otherindex_z
      7: selected_i / row
      8: selected_j / col
      9: progress_bar% [0-100]
    */

    int n = points.size();
    int m0 = 0, m1 = 0;
    if (n > 0) {
      m0 = points[0].size();
    }
    if (n > 1) {
      m1 = points[1].size();
    }

    // extra_params[0] = areas[0];
    // extra_params[1] = areas[1];
    
    if (n > 0 && m0 > PALMBASE_IDX) {
      if (std::get<0>(points[0][PALMBASE_IDX]) > 0 && std::get<1>(points[0][PALMBASE_IDX])) {
        extra_params[2] = std::get<0>(points[0][PALMBASE_IDX]);
        extra_params[3] = std::get<1>(points[0][PALMBASE_IDX]);
      
        // std::cerr << "setting palmbase coords (" << std::get<0>(points[0][PALMBASE_IDX]) << "," << std::get<1>(points[0][PALMBASE_IDX]) << ")\n";
      }
    }

    std::cerr << "initiator_default params strict:" << strict << " n:" << n << " m0:" << m0 << "\n";

    if (n > 1 && m1 > INDEXTOP_IDX && points[1].size() > INDEXTOP_IDX && areas[1] > SMALLAREA_THRESHOLD) {
        extra_params[4] = std::get<0>(points[1][INDEXTOP_IDX]);
        extra_params[5] = std::get<1>(points[1][INDEXTOP_IDX]);
        extra_params[6] = std::get<2>(points[1][INDEXTOP_IDX]);
    } else if (!strict && n > 0 && m0 > INDEXTOP_IDX && points[0].size() > INDEXTOP_IDX && areas[0] > SMALLAREA_THRESHOLD) {
        extra_params[4] = std::get<0>(points[0][INDEXTOP_IDX]);
        extra_params[5] = std::get<1>(points[0][INDEXTOP_IDX]);
        extra_params[6] = std::get<2>(points[0][INDEXTOP_IDX]);
        std::cerr << "initiator_default setting otherindex coord extra_params[4]:" << extra_params[4] << " extra_params[5]:" << extra_params[5] << "\n";
    }
    
    return extra_params;
}
