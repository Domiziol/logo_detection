#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Segment1.h"
#include <list>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

using PointsList = std::list<cv::Point2i>;
using PointsQueue = std::list<cv::Point2i>;
using SegmentList = std::list<Segment>;

class PerformSegmentation {
public:
	PerformSegmentation(cv::Mat& i);
	cv::Mat I;
	SegmentList segments;
	SegmentList P, Z, U;
	std::vector<cv::Point2i> minP, maxP, minZ, maxZ, minU, maxU;

	cv::Mat& return_img();
	// run all steps
	void perform();

	// get all pixels that are part of this segment
	void fill_segment(int x, int y, cv::Scalar color);

	// divide picture into segments
	void create_segments();

	// if segments area is too small or too big - remove segment
	void delete_segment_by_area();

	// calculate moment invariants "niezmienniki momentowe"
	void calc_NM_of_segments();

	// only for develompent - save some data to txt file
	void save_NM_to_file();

	// find segments that fit each letter
	void find_PZU_letters();

	// for each letter find its bounding box coordinates
	void find_bounding_boxes_for_letters();

	// perform recognition algorithm
	cv::Mat& recognize(cv::Mat& I);
};