#include "handlers.h"



/*
    copied from mediapipe/util/landmarks_to_render_data_calculator.cc
*/

namespace handlers {
namespace util {

inline double Remap(double x, double lo, double hi, double scale) {
  return (x - lo) / (hi - lo + 1e-6) * scale;
}


void GetMinMaxZ(
    const std::vector<std::tuple<double, double, double>>& landmarks, 
    double* z_min, double* z_max) {

  *z_min = std::numeric_limits<double>::max();
  *z_max = std::numeric_limits<double>::min();
  for (int i = 0; i < landmarks.size(); ++i) {
    const std::tuple<double, double, double> & landmark = landmarks.at(i);
    *z_min = std::min(std::get<2>(landmark), *z_min);
    *z_max = std::max(std::get<2>(landmark), *z_max);
  }
}


void SetColorSizeValueFromZ(
  double z, double z_min, double z_max,
  int *color_scale,
  double min_depth_circle_thickness,
  double max_depth_circle_thickness,
  int *size_scale) {
  const int color_value = 255 - static_cast<int>(Remap(z, z_min, z_max, 255));
  *color_scale = color_value;

  const double scale = max_depth_circle_thickness - min_depth_circle_thickness;
  const int thickness = static_cast<int>(
      min_depth_circle_thickness + (1.f - Remap(z, z_min, z_max, 1)) * scale);
  *size_scale = thickness;
}


} // namespace util
} // namespace handlers