#include "initiators.h"
#include <algorithm>


double fabs(double u) {
    if (u < 0) return -u;
    return u;
}

//------------------------------------------------------------------

bool cmp(const std::tuple<double, double, double>& a, const std::tuple<double, double, double>& b) {
    return std::get<0>(a) < std::get<0>(b) || (std::get<0>(a) == std::get<0>(b) && std::get<1>(a) < std::get<1>(b));
}

bool cw(const std::tuple<double, double, double>& a, const std::tuple<double, double, double>& b, const std::tuple<double, double, double>& c) {
    return std::get<0>(a)*(std::get<1>(b)-std::get<1>(c))+std::get<0>(b)*(std::get<1>(c)-std::get<1>(a))+std::get<0>(c)*(std::get<1>(a)-std::get<1>(b)) < 0;
}

bool ccw(const std::tuple<double, double, double>& a, const std::tuple<double, double, double>& b, const std::tuple<double, double, double>& c) {
    return std::get<0>(a)*(std::get<1>(b)-std::get<1>(c))+std::get<0>(b)*(std::get<1>(c)-std::get<1>(a))+std::get<0>(c)*(std::get<1>(a)-std::get<1>(b)) > 0;
}

void convex_hull(std::vector<std::tuple<double, double, double>>& a) {
    if (a.size() == 1)
        return;

    std::sort(a.begin(), a.end(), &cmp);
    std::tuple<double, double, double> p1 = a[0], p2 = a.back();
    std::vector<std::tuple<double, double, double>> up, down;
    up.push_back(p1);
    down.push_back(p1);
    for (int i = 1; i < (int)a.size(); i++) {
      if (i == a.size() - 1 || cw(p1, a[i], p2)) {
          while (up.size() >= 2 && !cw(up[up.size()-2], up[up.size()-1], a[i]))
              up.pop_back();
          up.push_back(a[i]);
      }
      if (i == a.size() - 1 || ccw(p1, a[i], p2)) {
          while(down.size() >= 2 && !ccw(down[down.size()-2], down[down.size()-1], a[i]))
              down.pop_back();
          down.push_back(a[i]);
      }
    }

    a.clear();
    for (int i = 0; i < (int)up.size(); i++)
        a.push_back(up[i]);
    for (int i = down.size() - 2; i > 0; i--)
        a.push_back(down[i]);
}

//------------------------------------------------------------------


//------------------------------------------------------------------
double area(const std::vector<std::tuple<double, double, double>>& fig) {
    double res = 0;
    for (unsigned i = 0; i < fig.size(); i++) {
        std::tuple<double, double, double> p = i ? fig[i - 1] : fig.back();
        std::tuple<double, double, double> q = fig[i];
        res += (std::get<0>(p) - std::get<0>(q)) * (std::get<1>(p) + std::get<1>(q));
    }
    return fabs(res) / 2;
}

//------------------------------------------------------------------
