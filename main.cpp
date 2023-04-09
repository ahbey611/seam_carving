#if 1

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "seam_carving.h"

using namespace std;
using namespace cv;

//进行转义
void tackle_path(string& path)
{
	string s;

	for (int i = 0;i < path.size();i++)
	{
		if (path[i] == '"')
			continue;

		if (path[i] == char(92))
		{
			s.push_back('/');
			continue;
		}

		s.push_back(path[i]);
	}

	path = s;
}

int main()
{	
	cout << "Plese input a path: (relative path or absolute path)" << endl;
	string path;
	cin >> path;

	tackle_path(path);
	Mat input = imread(path);
	Mat output;

	cout << "input image size: " << input.cols << " x " << input.rows << endl;
	SEAM_CARVING seam_carving(input, output, input.cols / 2, input.rows / 2);
	cout <<endl<< "seam carving done, you can check in you current folder." << endl;
	cout << "output image name: output.jpg" << endl;
	cout << "output image size: " << input.cols / 2 << " x " << input.rows / 2 << endl;

	return 0;
}

#endif