#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>


class Preprocessing {
public:
	Preprocessing(cv::Mat& img);

	cv::Mat image;

	cv::Mat& return_img();

	void conv_pixel_to_HSV(cv::Vec3b& pixel);

	void threshold(cv::Mat& I);

	void dilation(cv::Mat& I);
};