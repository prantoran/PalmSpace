#include "triggers.h"

// used by triggerdepth*

SupportPoints::SupportPoints() {}

void SupportPoints::init(int _size) {
    m_support_pts_size = _size;

    m_xcoords       = std::vector<type_t>(m_support_pts_size);
    m_xcoords_prev  = std::vector<type_t>(m_support_pts_size);
    m_ycoords       = std::vector<type_t>(m_support_pts_size);
    m_ycoords_prev  = std::vector<type_t>(m_support_pts_size);
    m_dx            = std::vector<type_t>(m_support_pts_size);
    m_dy            = std::vector<type_t>(m_support_pts_size);

    for (int i = 0; i < m_support_pts_size; i ++) {
        m_xcoords[i] = -1;
        m_ycoords[i] = -1;
    }
}


void SupportPoints::backup_prev() {
    for (int k = 0; k < m_support_pts_size; k ++) {
        m_xcoords_prev[k] = m_xcoords[k];
        m_ycoords_prev[k] = m_ycoords[k];
    }
}


void SupportPoints::update_support(
    int indices[], 
    const std::vector<std::tuple<double, double, double>> & coords,
    const Parameters & params
) {
    std::cerr << "update_support m_support_pts_size:" << m_support_pts_size << "\n";
    for (int i = 0; i < m_support_pts_size; i ++) {
        std::cerr << "indices[" << i << "]: " << indices[i] << "\n";
        m_xcoords[i] = (int)(params.m_frame_width  * std::get<0>(coords[indices[i]]));
        m_ycoords[i] = (int)(params.m_frame_height * std::get<1>(coords[indices[i]]));

        m_dx[i] = m_xcoords[i] - m_xcoords_prev[i];
        m_dy[i] = m_ycoords[i] - m_ycoords_prev[i];

        if (m_xcoords_prev[i] != -1) {
            m_xcoords[i] = (1-0.9)*m_xcoords[i] + (0.9)*m_xcoords_prev[i];
        }


        if (m_ycoords_prev[i] != -1) {
            m_ycoords[i] = (1-0.9)*m_ycoords[i] + (0.9)*m_ycoords_prev[i];
        }
    }
    // std::cerr << "update_support end\n";
}

// TODO overload to print using cout <<
void SupportPoints::print() {
    for (int i = 0; i < m_support_pts_size; i ++) {
        std::cout << "(" << m_xcoords[i] << ", " << m_ycoords[i] << ")\t";
    } std::cout << "\n";
}
 

 void SupportPoints::max_diff(type_t & diff) {
    diff = 0;
    for (int i = 0; i < m_support_pts_size; i ++) {
        diff = std::max(diff, std::fabs(m_dx[i]));
        diff = std::max(diff, std::fabs(m_dy[i]));
    }
 }