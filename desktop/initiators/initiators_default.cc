#include "initiators.h"

InitiatorDefault::InitiatorDefault() {
    name = "Initiator_Default";
    pointsConvex.resize(2);
    for (int i = 0; i < 2; i ++) {
      pointsConvex[i] = std::vector<std::tuple<double, double, double>> (21);
    }
    strict = false;
}

InitiatorDefault::~InitiatorDefault() {
  std::cout << "default initiator killed\n";
}

bool InitiatorDefault::inspect(
        std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params) {
    // may modify the position of vectors within points

    for (int i = 0; i < 2; i ++) {
      pointsConvex[i].resize(points[i].size());
      for (int j = 0; j < points[i].size(); j ++) {
        pointsConvex[i][j] = points[i][j];
      }
    }

    valid[0] = true;
    valid[1] = true;

    for (int i = 0; i < 2; i ++) {
      areas[i] = 0;
      convex_hull(pointsConvex[i]);
      areas[i] = area(pointsConvex[i]);

      if (areas[i] < SMALLAREA_THRESHOLD) {
        valid[i] = false;
      }
    }

    params.m_valid_points[0] = valid[0];
    params.m_valid_points[1] = valid[1];

    show_display = true;
    if (areas[handedness::LEFT] < AREA_THRESHOLD) {
      show_display = false;
    }

    if (areas[handedness::LEFT] < SMALLAREA_THRESHOLD) {
      valid[0] = false;
      valid[1] = false;
    }

    return show_display;
}


void InitiatorDefault::params(
  const std::vector<std::vector<std::tuple<double, double, double>>> & points,
  Parameters & params) {
  
  std::cerr << "initiator default params()\n";

  // params() to be called if inspect() returns true

  int valid_n = 0;
  for (int i = 0; i < points.size(); i ++) {
    if (valid[i]) {
      valid_n ++;
    }
  }

  params.set_total_hands(valid_n);

  int m0 = -1, m1 = -1;
    
  if (params.total_hands_detected() > 0) {
    m0 = points[handedness::LEFT].size();
  }

  // std::cout << "initiator_default params tot hands detected:" << params.total_hands_detected() << "\n";
  if (params.total_hands_detected() >= 2) {
    m1 = points[1-handedness::LEFT].size();
  }

  std::cerr << "set totatl hands detected\n";
  
  if (m0 > PALMBASE_IDX) {
    if (
      std::get<0>(points[handedness::LEFT][PALMBASE_IDX]) > 0 && 
      std::get<1>(points[handedness::LEFT][PALMBASE_IDX]) > 0) {
      
      params.set_palmbase(points[handedness::LEFT][PALMBASE_IDX]);
    }
  }

  if (m1 > PALMBASE_IDX) {
    if (
      std::get<0>(points[1][PALMBASE_IDX]) > 0 && 
      std::get<1>(points[1][PALMBASE_IDX]) > 0) {
      
      params.set_palmbase_right(points[1][PALMBASE_IDX]);
    }
  }

  if (m0 > INDEXBASE_IDX) {
    if (
      std::get<0>(points[handedness::LEFT][INDEXBASE_IDX]) > 0 && 
      std::get<1>(points[handedness::LEFT][INDEXBASE_IDX]) > 0) {

      params.set_indexbase(points[handedness::LEFT][INDEXBASE_IDX]);
    }
    
  }

  std::cerr << "setting idx\n";

  int idx = -1;
  switch(params.total_hands_detected()) {
    case 2:
      if (m1 > INDEXTOP_IDX && valid[1-handedness::LEFT]) {
        idx = 1-handedness::LEFT;
      }
      break;

    case 1:
      if (params.is_static() && m0 > INDEXTOP_IDX && valid[handedness::LEFT]) { // used for static displays
        idx = handedness::LEFT;
      }
      break;

    default:
      break;
  }

  std::cerr << "reseting cursor id\n";
  
  params.m_cursor_id = -1;

  if (idx >= 0) {
    if (areas[idx] > SMALLAREA_THRESHOLD) {
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
  }
}