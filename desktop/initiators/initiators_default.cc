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


void InitiatorDefault::params(
  const std::vector<std::vector<std::tuple<double, double, double>>> & points,
  ExtraParameters & parameters) {
  
  // params() to be called after inspect() returns true
  
  int n = points.size();
  int m0 = 0, m1 = 0;
  if (n > 0) {
    m0 = points[0].size();
  }
  if (n > 1) {
    m1 = points[1].size();
  }
  
  if (n > 0 && m0 > PALMBASE_IDX) {
    if (std::get<0>(points[0][PALMBASE_IDX]) > 0 && std::get<1>(points[0][PALMBASE_IDX])) {
      parameters.set_palmbase(points[0][PALMBASE_IDX]);
    }
  }

  if (n > 1 && m1 > INDEXTOP_IDX && points[1].size() > INDEXTOP_IDX && areas[1] > SMALLAREA_THRESHOLD) {
      parameters.set_indexfinger(points[1][INDEXTOP_IDX]);
  } else if (!strict && n > 0 && m0 > INDEXTOP_IDX && points[0].size() > INDEXTOP_IDX && areas[0] > SMALLAREA_THRESHOLD) {
      parameters.set_indexfinger(points[0][INDEXTOP_IDX]);
  }
}