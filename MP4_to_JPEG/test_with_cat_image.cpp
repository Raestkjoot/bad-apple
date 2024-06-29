#include <iostream>

#include "opencv2\opencv.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui.hpp"

int main_butNotAnyMore() {
	std::cout << CV_VERSION << std::endl;

	cv::Mat img = cv::imread("Data/cat.jpg");
	if (img.empty())
		return 0;

	cv::imshow("Display window", img);
	int k = cv::waitKey(0);

	return 0;
}