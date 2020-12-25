
#include "config.h"


Range::Range() {
    m_lower_bound = 0;
    m_upper_bound = 0;
}


Range::Range(number_t _lb, number_t _ub) {
    m_lower_bound = _lb;
    m_upper_bound = _ub;
}



bool Range::isInside(number_t val) {
    return (val >= m_lower_bound && val <= m_upper_bound);
}


void Range::print() {
    std::cout << "lower_bound:" << m_lower_bound << " upper_bound:" << m_upper_bound << "\n";
}
