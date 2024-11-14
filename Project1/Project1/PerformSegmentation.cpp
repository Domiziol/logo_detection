#include "PerformSegmentation.h"

PerformSegmentation::PerformSegmentation(cv::Mat& i) : I(i) { perform(); };

cv::Mat& PerformSegmentation::return_img() {
	return I;
}

void PerformSegmentation::perform() {
	create_segments();
	delete_segment_by_area();
	calc_NM_of_segments();
	find_PZU_letters();
	find_bounding_boxes_for_letters();
}

void PerformSegmentation::fill_segment(int x, int y, cv::Scalar color) {
	PointsQueue points;
	points.push_back(cv::Point2i(y, x));
	cv::Point2i temp, top, bottom, left, right;

	while (!points.empty()) {
		// get first point from queue
		temp = points.front();
		points.pop_front();

		I.at<cv::Vec3b>(temp)[0] = (uchar)color[0];
		I.at<cv::Vec3b>(temp)[1] = (uchar)color[1];
		I.at<cv::Vec3b>(temp)[2] = (uchar)color[2];
		segments.back().points_range.push_back(temp);
		segments.back().area += 1;

		left = cv::Point2i(temp.x - 1, temp.y);
		right = cv::Point2i(temp.x + 1, temp.y);
		top = cv::Point2i(temp.x, temp.y - 1);
		bottom = cv::Point2i(temp.x, temp.y + 1);

		if (left.x >= 0 && I.at<cv::Vec3b>(left)[0] == 255 && I.at<cv::Vec3b>(left)[1] == 255 && I.at<cv::Vec3b>(left)[2] == 255 && find(points.begin(), points.end(), left) == points.end()) {
			points.push_back(left);
		}
		if (right.x < I.cols && I.at<cv::Vec3b>(right)[0] == 255 && I.at<cv::Vec3b>(right)[1] == 255 && I.at<cv::Vec3b>(right)[2] == 255 && find(points.begin(), points.end(), right) == points.end()) {
			points.push_back(right);
		}
		if (top.y >= 0 && I.at<cv::Vec3b>(top)[0] == 255 && I.at<cv::Vec3b>(top)[1] == 255 && I.at<cv::Vec3b>(top)[2] == 255 && find(points.begin(), points.end(), top) == points.end()) {
			points.push_back(top);
		}
		if (bottom.y < I.rows && I.at<cv::Vec3b>(bottom)[0] == 255 && I.at<cv::Vec3b>(bottom)[1] == 255 && I.at<cv::Vec3b>(bottom)[2] == 255 && find(points.begin(), points.end(), bottom) == points.end()) {
			points.push_back(bottom);
		}
	}
}

void PerformSegmentation::create_segments() {
	cv::Mat_<cv::Vec3b> _I = I;
	cv::Scalar color;

	for (int i = 0; i < _I.rows; ++i) {
		for (int j = 0; j < _I.cols; ++j) {
			if (_I.at<cv::Vec3b>(i, j)[0] == 255 && _I.at<cv::Vec3b>(i, j)[1] == 255 && _I.at<cv::Vec3b>(i, j)[2] == 255) {
				//ok, we have white pixel, lets start filling the whole segment
				color = cv::Scalar(255, 255, 0);
				segments.push_back(Segment(color));
				fill_segment(i, j, color);
			}
		}
	}
	I = _I;
}

void PerformSegmentation::delete_segment_by_area() {

	for (SegmentList::iterator it = segments.begin(); it != segments.end(); ++it) {
		if (it->area < 400 || it->area > 20000) {
			for (PointsList::iterator pkt = it->points_range.begin(); pkt != it->points_range.end(); ++pkt) {
				I.at<cv::Vec3b>(*pkt)[0] = 0;
				I.at<cv::Vec3b>(*pkt)[1] = 0;
				I.at<cv::Vec3b>(*pkt)[2] = 0;
			}
		}
	}
	segments.erase(std::remove_if(segments.begin(), segments.end(), [](auto& el) {
		if (el.area < 400 || el.area > 20000) return true;
		else return false; }), segments.end());
}

void PerformSegmentation::calc_NM_of_segments() {
	for (SegmentList::iterator it = segments.begin(); it != segments.end(); ++it) {
		it->inv_moments();
	}
}
void PerformSegmentation::save_NM_to_file() {

	std::ofstream f;
	f.open("seg6.txt", std::ios::app);
	if (f.is_open()) {
		//f << "Nr.; NM1; NM3; NM7 \n";
		int i = 1;
		for (SegmentList::iterator it = segments.begin(); it != segments.end(); ++it) {
			
			f << std::to_string(i) +";"+ std::to_string(it->NM1) + ";" + std::to_string(it->NM2) + ";" + std::to_string(it->NM3) + ";" + std::to_string(it->NM7) + "\n";
			++i;
		}
		f << "\n";
	}
	f.close();
}

void PerformSegmentation::find_bounding_boxes_for_letters() {
	
	int Xmax=0, Xmin=0, Ymax=0, Ymin=0;

	if (!P.empty()) {

		for (SegmentList::iterator it = P.begin(); it != P.end(); ++it) {
			Xmax = Ymax = 0;
			Ymin = I.rows;
			Xmin = I.cols;

			for (PointsList::iterator p = it->points_range.begin(); p != it->points_range.end(); ++p) {
				Xmax = std::max(Xmax, p->x);
				Xmin = std::min(Xmin, p->x);
				Ymax = std::max(Ymax, p->y);
				Ymin = std::min(Ymin, p->y);
			}
			minP.push_back(cv::Point2i(Xmin, Ymin));
			maxP.push_back(cv::Point2i(Xmax, Ymax));
		}
	}
	if (!Z.empty()) {
		for (SegmentList::iterator it = Z.begin(); it != Z.end(); ++it) {
			Xmax = Ymax = 0;
			Ymin = I.rows;
			Xmin = I.cols;

			for (PointsList::iterator p = it->points_range.begin(); p != it->points_range.end(); ++p) {
				Xmax = std::max(Xmax, p->x);
				Xmin = std::min(Xmin, p->x);
				Ymax = std::max(Ymax, p->y);
				Ymin = std::min(Ymin, p->y);
			}
			minZ.push_back(cv::Point2i(Xmin, Ymin));
			maxZ.push_back(cv::Point2i(Xmax, Ymax));
		}
	}
	if (!U.empty()) {
		for (SegmentList::iterator it = U.begin(); it != U.end(); ++it) {
			Xmax = Ymax = 0;
			Ymin = I.rows;
			Xmin = I.cols;

			for (PointsList::iterator p = it->points_range.begin(); p != it->points_range.end(); ++p) {
				Xmax = std::max(Xmax, p->x);
				Xmin = std::min(Xmin, p->x);
				Ymax = std::max(Ymax, p->y);
				Ymin = std::min(Ymin, p->y);
			}
			minU.push_back(cv::Point2i(Xmin, Ymin));
			maxU.push_back(cv::Point2i(Xmax, Ymax));
		}
	}
}

cv::Mat& PerformSegmentation::recognize(cv::Mat& I) {
	cv::Mat_<cv::Vec3b> _I = I;
	int cxp, cyp, cxz, cyz, cxu, cyu;
	int dpz = 0 , dpu, dzu = 2000, d;
	int xmaxb, ymaxb, xminb, yminb;
	int iu=0, iz=0;

	for (int i = 0; i < P.size(); ++i) {
		
		xmaxb = ymaxb = 0;
		yminb = I.rows;
		xminb = I.cols;

		cxp = (maxP[i].x - minP[i].x) / 2 + minP[i].x;
		cyp = (maxP[i].y - minP[i].y) / 2 + minP[i].y;

		for (int j = 0; j < Z.size(); ++j) {
			cxz = (maxZ[j].x - minZ[j].x) / 2 + minZ[j].x;
			cyz = (maxZ[j].y - minZ[j].y) / 2 + minZ[j].y;

			dpz = (int)pow(pow(cxp - cxz, 2) + pow(cyp - cyz, 2), 0.5);

			dzu = 2000;
			iu = 0;
			for (int k = 0; k < U.size(); ++k) {
				
				cxu = (maxU[k].x - minU[k].x) / 2 + minU[k].x;
				cyu = (maxU[k].y - minU[k].y) / 2 + minU[k].y;

				d = (int)pow(pow(cxu - cxz, 2) + pow(cyu - cyz, 2), 0.5);
				if (dzu > d) {
					dzu = d;
					iu = k;
				}
			}
			
			if (std::abs(dpz - dzu) < 15) {
				iz = j;

				if (!U.empty() && !Z.empty()) {
					cxu = (maxU[iu].x - minU[iu].x) / 2 + minU[iu].x;
					cyu = (maxU[iu].y - minU[iu].y) / 2 + minU[iu].y;
					dpu = (int)pow(pow(cxu - cxp, 2) + pow(cyu - cyp, 2), 0.5);

					if (std::abs(dpu - dpz - dzu) < 5) {
						xmaxb = std::max(maxP[i].x, std::max(maxZ[iz].x, maxU[iu].x));
						xminb = std::min(minP[i].x, std::min(minZ[iz].x, minU[iu].x));

						ymaxb = std::max(maxP[i].y, std::max(maxZ[iz].y, maxU[iu].y));
						yminb = std::min(minP[i].y, std::min(minZ[iz].y, minU[iu].y));

						for (int r = yminb - 10; r <= ymaxb+10; ++r) {
							for (int c = xminb - 10; c < xminb - 5; ++c) {
								_I.at<cv::Vec3b>(r, c)[0] = 0;
								_I.at<cv::Vec3b>(r, c)[1] = 255;
								_I.at<cv::Vec3b>(r, c)[2] = 0;

								_I.at<cv::Vec3b>(r, c)[0] = 0;
								_I.at<cv::Vec3b>(r,c)[1] = 255;
								_I.at<cv::Vec3b>(r, c)[2] = 0;
							}
							for (int c = xmaxb + 5; c < xmaxb + 10; ++c) {
								_I.at<cv::Vec3b>(r, c)[0] = 0;
								_I.at<cv::Vec3b>(r, c)[1] = 255;
								_I.at<cv::Vec3b>(r, c)[2] = 0;

								_I.at<cv::Vec3b>(r, c)[0] = 0;
								_I.at<cv::Vec3b>(r, c)[1] = 255;
								_I.at<cv::Vec3b>(r, c)[2] = 0;
							}
						}
						for (int r = xminb - 10; r <= xmaxb+10; ++r) {
							for (int c = yminb - 10; c < yminb - 5; ++c) {
								_I.at<cv::Vec3b>(c, r)[0] = 0;
								_I.at<cv::Vec3b>(c, r)[1] = 255;
								_I.at<cv::Vec3b>(c, r)[2] = 0;

								_I.at<cv::Vec3b>(c, r)[0] = 0;
								_I.at<cv::Vec3b>(c, r)[1] = 255;
								_I.at<cv::Vec3b>(c, r)[2] = 0;
							}
							for (int c = ymaxb + 5; c < ymaxb + 10; ++c) {
								_I.at<cv::Vec3b>(c, r)[0] = 0;
								_I.at<cv::Vec3b>(c, r)[1] = 255;
								_I.at<cv::Vec3b>(c, r)[2] = 0;

								_I.at<cv::Vec3b>(c, r)[0] = 0;
								_I.at<cv::Vec3b>(c, r)[1] = 255;
								_I.at<cv::Vec3b>(c, r)[2] = 0;
							}
						}
					}
				}
			}
		}
	}

	I = _I;
	return I;
}


void PerformSegmentation::find_PZU_letters() {

	for (SegmentList::iterator it = segments.begin(); it != segments.end(); ++it) {
/*P*/
		if (it->NM1 > 0.17 && it->NM1 < 0.236 && it->NM2 > 0.0004 && it->NM2 < 0.0046 &&
			it->NM3 > 0.00085 && it->NM3 < 0.003 && it->NM7 > 0.007 && it->NM7 < 0.014) {

			for (PointsList::iterator pkt = it->points_range.begin(); pkt != it->points_range.end(); ++pkt) {
				I.at<cv::Vec3b>(*pkt)[0] = 40;
				I.at<cv::Vec3b>(*pkt)[1] = 40;
				I.at<cv::Vec3b>(*pkt)[2] = 40;
			}

			P.push_back(*it);
			continue;
		}
/*Z*/	else if (it->NM1 > 0.367 && it->NM1 < 0.62 && it->NM2 > 0.07 && it->NM2 < 0.3 &&
			it->NM3 > 0 && it->NM3 < 0.004 && it->NM7 > 0.009 && it->NM7 < 0.028) {

			for (PointsList::iterator pkt = it->points_range.begin(); pkt != it->points_range.end(); ++pkt) {
				I.at<cv::Vec3b>(*pkt)[0] = 100;
				I.at<cv::Vec3b>(*pkt)[1] = 120;
				I.at<cv::Vec3b>(*pkt)[2] = 200;
			}

			Z.push_back(*it);
			continue;
		}
/*U*/   else if (it->NM1 > 0.22 && it->NM1 < 0.35 && it->NM2 > 0.0001 && it->NM2 < 0.005 &&
			it->NM3 > 0 && it->NM3 < 0.002 && it->NM7 > 0.0012 && it->NM7 < 0.028) {

			for (PointsList::iterator pkt = it->points_range.begin(); pkt != it->points_range.end(); ++pkt) {
				I.at<cv::Vec3b>(*pkt)[0] = 150;
				I.at<cv::Vec3b>(*pkt)[1] = 20;
				I.at<cv::Vec3b>(*pkt)[2] = 80;
			}

			U.push_back(*it);
			continue;
		}
		else {
			for (PointsList::iterator pkt = it->points_range.begin(); pkt != it->points_range.end(); ++pkt) {
				I.at<cv::Vec3b>(*pkt)[0] = 0;
				I.at<cv::Vec3b>(*pkt)[1] = 0;
				I.at<cv::Vec3b>(*pkt)[2] = 0;
			}
		}
	}

	segments.erase(std::remove_if(segments.begin(), segments.end(), [this](auto& el) {
		if (el.NM1 > 0.17 && el.NM1 < 0.26 && el.NM2 > 0.0002 && el.NM2 < 0.005 &&
			el.NM3 > 0.0003 && el.NM3 < 0.0045 && el.NM7 > 0.005 && el.NM7 < 0.02)
			return false;
		else if (el.NM1 > 0.367 && el.NM1 < 0.62 && el.NM2 > 0.07 && el.NM2 < 0.3 &&
			el.NM3 > 0 && el.NM3 < 0.003 && el.NM7 > 0.009 && el.NM7 < 0.028)
			return false;
		else if (el.NM1 > 0.23 && el.NM1 < 0.3 && el.NM2 > 0.00005 && el.NM2 < 0.0045 &&
			el.NM3 > 0 && el.NM3 < 0.0025 && el.NM7 > 0.00132 && el.NM7 < 0.028)
			return false;
		else return true;
		}), segments.end());


}
