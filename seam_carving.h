#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <vector>
#include <limits.h>

using namespace std;
using namespace cv;

class SEAM_CARVING
{
	
private:

	struct Point {
		float val;
		int path;
		Point() :val(0), path(0) {}
		Point(float v, int p) :val(v), path(p) {}
		friend ostream& operator<<(ostream& out, const Point& p)
		{
			//out << p.path;
			out << p.val;
			return out;
		}
	};

	int reduce_cols;
	int reduce_rows;
	Mat input;
	Mat output;
	int num = 0;
	

public:
	SEAM_CARVING();
	SEAM_CARVING(Mat& input, Mat& output, int reduce_cols, int reduce_rows);
	void get_energy(Mat& energy);
	void get_min_energy_cost(Mat& energy, vector<vector<Point>>& matrix);
	void get_seam(vector<vector<Point>>& matrix, vector<int>& seam, float& cost);
	void remove_seam(vector<int>& seam, Mat& des);
};
