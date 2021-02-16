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
        std::vector<std::vector<std::tuple<double, double, double>>> & points,
        Parameters & params) {
    // may modify the position of vectors within points
    
    valid[0] = true;
    valid[1] = true;


    for (int i = 0; i < 2; i ++) {
        pointsConvex[i].clear();
        for (int j = 0; valid[i] && j < points[i].size(); j ++) {
            auto & u = points[i][j];
            pointsConvex[i].emplace_back(u);
            // if (std::get<0>(u) < 0.01 && std::get<1>(u) < 0.01) {
            //   valid[i] = false;
            // }

            // if (std::get<0>(u) > 0.99 && std::get<1>(u) > 0.99) {
            //   valid[i] = false;
            // }
        }
    }

    for (int i = 0; i < 2; i ++) {
      areas[i] = 0;
      if (points[i].size() > 2) {
        convex_hull(pointsConvex[i]);
        areas[i] = area(pointsConvex[i]);
      }

      if (areas[i] < SMALLAREA_THRESHOLD) {
        valid[i] = false;
      }
    }

    params.m_base_id = 0;
    if (valid[1] && params.hand[1] == handedness::LEFT) {      
      std::cerr << "switching\n";
      params.m_base_id = 1;
    }

    std::cerr << "base_id:" << params.m_base_id << "\thand:" << params.hand[params.m_base_id] << "\n";
    
    show_display = true;
    if (areas[params.m_base_id] < AREA_THRESHOLD) {
      show_display = false;
    }

    if (areas[params.m_base_id] < SMALLAREA_THRESHOLD) {
      valid[0] = false;
      valid[1] = false;
    }

    return show_display;
}


void InitiatorDefault::params(
  const std::vector<std::vector<std::tuple<double, double, double>>> & points,
  Parameters & params) {
  
  // std::cerr << "initiator default params()\n";

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
    m0 = points[params.m_base_id].size();
  }

  if (params.total_hands_detected() >= 2) {
    m1 = points[1-params.m_base_id].size();
  }
  
  if (m0 > PALMBASE_IDX) {
    if (
      std::get<0>(points[params.m_base_id][PALMBASE_IDX]) > 0 && 
      std::get<1>(points[params.m_base_id][PALMBASE_IDX]) > 0) {
      
      params.set_palmbase(points[params.m_base_id][PALMBASE_IDX]);
    }
  }

  if (m0 > INDEXBASE_IDX) {
    if (
      std::get<0>(points[params.m_base_id][INDEXBASE_IDX]) > 0 && 
      std::get<1>(points[params.m_base_id][INDEXBASE_IDX]) > 0) {

      params.set_indexbase(points[params.m_base_id][INDEXBASE_IDX]);
    }
  }

  int idx = -1;
  switch(params.total_hands_detected()) {
    case 2:
      if (m1 > INDEXTOP_IDX && valid[1-params.m_base_id]) {
        idx = 1-params.m_base_id;
      }
      break;

    case 1:
      if (params.is_static() && m0 > INDEXTOP_IDX && valid[params.m_base_id]) { // used for static displays
        idx = params.m_base_id;
      }
      break;

    default:
      break;
  }
  
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