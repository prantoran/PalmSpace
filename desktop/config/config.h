#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <tuple>

constexpr int PSIZE = 10;

class ExtraParameters {
    public:
    std::vector<double> extra_params;
    int psize;

    std::tuple<double, double, double> indexbase, palmbase;
    
    /*
      0: min_ws
      1: min_hs
      2: palmbase_x
      3: palmbase_y
      4: otherindex_x
      5: otherindex_y
      6: otherindex_z
      7: selected_i / row
      8: selected_j / col
      9: progress_bar% [0-100]
    */

    ExtraParameters();
    ~ExtraParameters();

    void set(int i, double v);
    void set(const std::vector<double> & p);

    double at(int i);

    void reset();
    
    void set_indexfinger(const std::tuple<double, double, double> & p);
    void get_indexfinger(double &x, double &y);
    bool is_set_indexfinger();

    void get_palmbase(double &x, double &y);
    void get_palmbase(std::tuple<double, double, double> & p);
    void set_palmbase(const std::tuple<double, double, double> & p);
    void set_palmbase(double x, double y);

    void get_indexbase(double &x, double &y);
    void get_indexbase(std::tuple<double, double, double> & p);
    void set_indexbase(const std::tuple<double, double, double> & p);
    void set_indexbase(double x, double y);
};

#endif