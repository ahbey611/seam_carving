#if 1

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "seam_carving.h"

using namespace std;
using namespace cv;

int main()
{
	Mat input = imread("D:/opencv455/code/bin/origin.jpg");
	Mat output;

	cout << input.cols << " " << input.rows << endl;
	//transpose(input, input);
	//flip(input, input,1);
	// 
	//imshow("input", input);
	//imwrite("D:/opencv455/code/bin/input.jpg", input);
	//waitKey(1000);
	SEAM_CARVING seam_carving(input, output, input.cols / 2, input.rows / 2);
	//cv::imwrite("D:/opencv455/code/bin/test_result.jpg", output);
	//cv::imshow("1", output);
	//cv::waitKey();
	return 0;
}


#endif