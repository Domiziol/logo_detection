#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <list>

using PointsList = std::list<cv::Point2i>;

class Segment {
public:
	Segment(cv::Scalar col);
	cv::Scalar color;
	PointsList points_range;
	double area = 0;
	
	// momenty i niezmienniki
    double m00;
    double m01;
    double m10;
    double m02;
    double m20;
    double m03;
    double m30;
    double m11;
    double m21;
    double m12;

    double ci;
    double cj;

    double M02;
    double M20;
    double M03;
    double M30;
    double M11;
    double M21;
    double M12;

    double NM1;
    double NM2;
    double NM3;
    double NM7;

    double calculate_mpq(int p, int q);
    void inv_moments();

};
