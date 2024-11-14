#include "Segment1.h"

Segment::Segment(cv::Scalar col) : color(col) {
    m00 = 0;
    m01 = 0;
    m10 = 0;
    m02 = 0;
    m20 = 0;
    m03 = 0;
    m30 = 0;
    m11 = 0;
    m21 = 0;
    m12 = 0;

    ci = 0;
    cj = 0;

    M02 = 0;
    M20 = 0;
    M03 = 0;
    M30 = 0;
    M11 = 0;
    M21 = 0;
    M12 = 0;

    NM1 = 0;
    NM2 = 0;
    NM3 = 0;
    NM7 = 0;
};

double Segment::calculate_mpq(int p, int q) {
    double m = 0;
    for (PointsList::iterator it = points_range.begin(); it != points_range.end(); ++it) {
        m += pow(it->y+1, p) * pow(it->x+1, q);
    }
    return m;
}

void Segment::inv_moments() {
    m00 = area;
    m01 = calculate_mpq(0, 1);
    m02 = calculate_mpq(0, 2);
    m10 = calculate_mpq(1, 0);
    m03 = calculate_mpq(0, 3);
    m30 = calculate_mpq(3, 0);
    m12 = calculate_mpq(1, 2);
    m11 = calculate_mpq(1, 1);
    m21 = calculate_mpq(2, 1);
    m20 = calculate_mpq(2, 0);

    ci = m10 / m00;
    cj = m01 / m00;
   

    M02 = m02 - m01 * m01 / m00;
    M20 = m20 - m10 * m10 / m00;
    M03 = m03 - 3 * m02 * cj + 2 * m01 * cj * cj;
    M30 = m30 - 3 * m20 * ci + 2 * m10 * ci * ci;
    M12 = m12 - 2 * m11 * cj - m02 * ci + 2 * m10 * cj * cj;
    M21 = m21 - 2 * m11 * ci - m20 * cj + 2 * m01 * ci * ci;
    M11 = m11 - m10 * m01 / m00;

    NM1 = (M20 + M02) / (m00 * m00);
    NM2 = (pow(M20 - M02, 2) + 4 * M11 * M11) / pow(m00, 4);
    NM3 = (pow(M30 - 3 * M12, 2) + pow(3 * M21 - M03, 2)) / pow(m00, 5);
    NM7 = (M20 * M02 - M11 * M11) / pow(m00, 4);
}