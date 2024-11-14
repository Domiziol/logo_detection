#include "Preprocessing.h"

Preprocessing::Preprocessing(cv::Mat& img) {
	image = img.clone();
};


cv::Mat& Preprocessing::return_img() {
	threshold(image);
	return image;
}

void Preprocessing::conv_pixel_to_HSV(cv::Vec3b& pixel) {
	const uchar R = pixel[2];
	const uchar G = pixel[1];
	const uchar B = pixel[0];
	uchar value = 0;
	uchar Cmax = 0;
	uchar Cmin = 0;
	int hue = 0;
	uchar sat = 0;
	uchar delta = 0;

	Cmax = std::max(R, std::max(G, B));
	Cmin = std::min(R, std::min(G, B));
	delta = Cmax - Cmin;

	// value
	value = Cmax;

	//saturation
	if (Cmax == 0)
		sat = 0;
	else sat = delta * 255 / value;

	//hue
	if (delta == 0) {
		hue = 0;
	}
	else if (Cmax == R) {
		hue = (G - B) * 60 / delta;
	}
	else if (Cmax == G) {
		hue = ((B - R) * 60 / delta) + 120;
	}
	else if (Cmax == B) {
		hue = ((R - G) * 60 / delta) + 240;
	}

	if (hue < 0)
		hue += 360;

	//thresholding no. 1
	if (hue > 70 && hue < 240) {	
		if (sat > 40 && sat < 255) {	
			if (value > 60 && value < 200) 
				pixel = { 0, 0, 0 };
		}
	}
	else pixel = { 255, 255, 255 };
}

void Preprocessing::threshold(cv::Mat& I) {
	int maksik = 0;
	int h = 0, s = 0, v = 0;
	int a = 0, b = 0;

	for (int i = 0; i < I.rows; ++i) {
		for (int j = 0; j < I.cols; ++j) {
			conv_pixel_to_HSV(I.at<cv::Vec3b>(i, j));
		}
	}

	cv::Mat_<cv::Vec3b> R = I;
	for (int i = 0; i < R.rows; ++i) {
		for (int j = 0; j < R.cols; ++j) {

			//thresholding no.2
			if (static_cast<int>(R.at<cv::Vec3b>(i, j)[0]) == 0 && static_cast<int>(R.at<cv::Vec3b>(i, j)[1]) == 0 && static_cast<int>(R.at<cv::Vec3b>(i, j)[2]) == 0) {
				R(i, j)[0] = 255;
				R(i, j)[1] = 255;
				R(i, j)[2] = 255;
			}
			else {
				R(i, j)[0] = 0;
				R(i, j)[1] = 0;
				R(i, j)[2] = 0;
			}
		}
	}
	I = R;
}

// optional function
void Preprocessing::dilation(cv::Mat& I) {
	
	int size = 11;

	int l = size / 2;
	int sum = 0;
	cv::Mat_<cv::Vec3b> R = I;
	for (int i = l; i < R.rows - l; ++i) {
		for (int j = l; j < R.cols-l; ++j) {

			sum = 0.0;
			for (int k1 = 0; k1 < l; ++k1) {
				for (int k2 = 0; k2 < l; ++k2) {
					if (static_cast<int>(R.at<cv::Vec3b>(i + k1 - l, j + k2 - l)[0]) == 255 && static_cast<int>(R.at<cv::Vec3b>(i + k1 - l, j + k2 - l)[1]) == 255 && static_cast<int>(R.at<cv::Vec3b>(i + k1 - l, j + k2 - l)[2]) == 255) {
						sum += 1;
					}
				}
			}
			if (sum >0) {
				R.at<cv::Vec3b>(i, j)[0] = 255;
				R.at<cv::Vec3b>(i, j)[1] = 255;
				R.at<cv::Vec3b>(i, j)[2] = 255;
			}
		}
	}
	I = R;
}