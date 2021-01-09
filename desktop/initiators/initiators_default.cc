#include "initiators.h"

InitiatorDefault::InitiatorDefault() {
    name = "Initiator_Default";
    pointsConvex.resize(2);
    strict = false;
}

InitiatorDefault::~InitiatorDefault() {

  std::cout << "default initiator killed\n";
}

bool InitiatorDefault::inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points) {
    // may modify the position of vectors within points
    
    valid[0] = true;
    valid[1] = true;

    for (int i = 0; i < 2; i ++) {
        pointsConvex[i].clear();
        for (auto & u: points[i]) {
            pointsConvex[i].emplace_back(u);
            if (std::get<0>(u) < 0.1 && std::get<1>(u) < 0.1) {
              valid[i] = false;
            }

            if (std::get<0>(u) > 0.9 && std::get<1>(u) > 0.9) {
              valid[i] = false;
            }
        }
    }

    for (int i = 0; i < 2; i ++) {
      areas[i] = 0;
      if (points[i].size() > 2) {
        convex_hull(pointsConvex[i]);
        areas[i] = area(pointsConvex[i]);
      }
    }

    // if (areas[0] < areas[1]) {
    if (valid[1] && areas[1] > AREA_THRESHOLD && std::get<0>(points[1][17]) < std::get<0>(points[0][17])) {
      std::swap(points[0], points[1]);
      std::swap(areas[0], areas[1]);
      std::swap(valid[0], valid[1]);
    }
    

    show_display = true;
    if (areas[0] < AREA_THRESHOLD) {
      show_display = false;
      if (areas[0] < SMALLAREA_THRESHOLD) {
        valid[0] = false;
        valid[1] = false;
      }
    }

    return show_display;
}


void InitiatorDefault::params(
  const std::vector<std::vector<std::tuple<double, double, double>>> & points,
  Parameters & parameters) {
  
  // params() to be called if inspect() returns true

  int n = points.size();
  int valid_n = 0;
  for (int i = 0; i < n; i ++) {
    if (valid[i]) {
      valid_n ++;
    }
  }

  parameters.set_total_hands(valid_n);

  int m0 = 0, m1 = 0;
  if (n > 0) {
    m0 = points[0].size();
  }
  if (n > 1) {
    m1 = points[1].size();
  }
  
  if (n > 0) {
    if (m0 > PALMBASE_IDX) {
      if (std::get<0>(points[0][PALMBASE_IDX]) > 0 && std::get<1>(points[0][PALMBASE_IDX]) > 0) {
        parameters.set_palmbase(points[0][PALMBASE_IDX]);
      }
    }

    if (m0 > INDEXBASE_IDX) {
      if (std::get<0>(points[0][INDEXBASE_IDX]) > 0 && std::get<1>(points[0][INDEXBASE_IDX]) > 0) {
        parameters.set_indexbase(points[0][INDEXBASE_IDX]);
      }
    }
  }

  int idx = -1;
  if (n > 1 && m1 > INDEXTOP_IDX && valid[1]) {
    idx = 1;
  } else if (parameters.is_static() && n > 0 && m0 > INDEXTOP_IDX && valid[0]) { // used for static displays
    idx = 0;
  }

  parameters.m_cursor_id = -1;
  

  if (idx >= 0) {

    if (areas[idx] > SMALLAREA_THRESHOLD) {
      if (points[idx].size() > INDEXTOP_IDX) {
        parameters.m_cursor_id = idx;      
        parameters.set_primary_cursor(points[idx][INDEXTOP_IDX]);
      } else {
        parameters.m_primary_cursor.reset();
      }

      if (points[idx].size() > MIDDLEFINGERBASE) {
        parameters.m_cursor_middlebase_id = idx;      
        parameters.set_primary_cursor_middlefinger_base(points[idx][MIDDLEFINGERBASE]);
      } else {
        parameters.m_primary_cursor_middlefinger_base.reset();
      }
    }
  }
}