#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Segment1.h"
#include "Preprocessing.h"
#include "PerformSegmentation.h"
#include <iostream>
#include <string>


int main() {


	// Tutaj mozna zmieniac pliki
	std::string path = "pzu1.jpg";

	cv::Mat image = cv::imread(path);
	std::cout << "Czytam domyslny plik..." << std::endl;
	

	if (image.empty())
	{
		std::cout << "Puste zdjecie, koncze" << std::endl;
		return 1;
	}
	else {
		std::cout <<std::endl<< "Przetwarzam zdjecie..." << std::endl<<std::endl;
	}

	Preprocessing preproc = Preprocessing(image);
	cv::Mat processed = preproc.return_img();
		
	PerformSegmentation perseg = PerformSegmentation(processed);
		
	cv::Mat segmented = perseg.return_img();
	cv::Mat img = perseg.recognize(image);
		
	std::string name = "processed";
	cv::imshow(name, img);
	cv::waitKey(-1);

	return 0;
}