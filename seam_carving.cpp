#include "seam_carving.h"


SEAM_CARVING::SEAM_CARVING()
{
	reduce_cols = reduce_rows = 0;
}

SEAM_CARVING::SEAM_CARVING(Mat& source, Mat& desinition, int reduce_cols, int reduce_rows)
	:reduce_cols(reduce_cols), reduce_rows(reduce_rows)
{
	input = source.clone();

	int count_row = 0;
	int count_col = 0;
	int width = source.cols;
	int height = source.rows;

	//优先删除两个方向中能量最小的路径
	while (count_row < reduce_rows && count_col < reduce_cols)
	{
		//获取能量矩阵
		Mat energy;
		get_energy(energy);

		//竖直方向
		float cost_v = 0;
		vector<int> seam_v;//纵方向路径
		vector<vector<Point>> matrix_v;//最小代价矩阵
		get_min_energy_cost(energy, matrix_v);//计算竖直最小代价矩阵
		get_seam(matrix_v, seam_v, cost_v);//获取最短路径与其代价之和
		reverse(seam_v.begin(), seam_v.end());//由下往上找路径，因此把路径反转


	//水平方向 (相当于把图片旋转，再做竖直方向的计算，代码量减少)
		float cost_h = 0;
		vector<int> seam_h;//横方向路径
		vector<vector<Point>> matrix_h;//最小代价矩阵
		transpose(energy, energy);//转置
		flip(energy, energy, 1);//反转 flipCode>0水平翻转（沿Y轴翻转）(镜像翻转)
		get_min_energy_cost(energy, matrix_h);//计算竖直最小代价矩阵
		get_seam(matrix_h, seam_h, cost_h);
		reverse(seam_h.begin(), seam_h.end());


		//比较两个方向的最小代价，取min(cost_v,cost_h)作为要删除的那条线

		//直线代价<横线代价 -> 删除直线 即图片宽度-1
		if (cost_v < cost_h)
		{
			Mat cropped(height, width - 1, CV_8UC3);
			remove_seam(seam_v, cropped);
			cropped.copyTo(input);
			width--;
			count_col++;
		}

		//直线代价>=横线代价 -> 删除横线 即图片高度-1
		else
		{
			Mat cropped(width, height - 1, CV_8UC3);
			transpose(input, input);
			flip(input, input, 1);
			remove_seam(seam_h, cropped);
			transpose(cropped, cropped);
			flip(cropped, cropped, 1);
			cropped.copyTo(input);
			height--;
			count_row++;
		}

	}

	//删除的行数还不足够时，继续删除
	while (count_row < reduce_rows)
	{
		//获取能量矩阵
		Mat energy;
		get_energy(energy);

		//水平方向 (相当于把图片旋转，再做竖直方向的计算，代码量减少)
		float cost_h = 0;
		vector<int> seam_h;//横方向路径
		vector<vector<Point>> matrix_h;//最小代价矩阵
		transpose(energy, energy);//转置
		flip(energy, energy, 1);//反转 flipCode>0水平翻转（沿Y轴翻转）(镜像翻转)
		get_min_energy_cost(energy, matrix_h);//计算竖直最小代价矩阵
		get_seam(matrix_h, seam_h, cost_h);
		reverse(seam_h.begin(), seam_h.end());

		Mat cropped(width, height - 1, CV_8UC3);
		transpose(input, input);
		flip(input, input, 1);
		remove_seam(seam_h, cropped);
		transpose(cropped, cropped);
		flip(cropped, cropped, 1);
		cropped.copyTo(input);
		height--;
		count_row++;
	}

	while (count_col < reduce_cols)
	{
		//获取能量矩阵
		Mat energy;
		get_energy(energy);

		//竖直方向
		float cost_v = 0;
		vector<int> seam_v;//纵方向路径
		vector<vector<Point>> matrix_v;//最小代价矩阵
		get_min_energy_cost(energy, matrix_v);//计算竖直最小代价矩阵
		get_seam(matrix_v, seam_v, cost_v);//获取最短路径与其代价之和
		reverse(seam_v.begin(), seam_v.end());//由下往上找路径，因此把路径反转

		Mat cropped(height, width - 1, CV_8UC3);
		remove_seam(seam_v, cropped);
		cropped.copyTo(input);
		width--;
		count_col++;
	}

	//output = input;
	input.copyTo(output);

	/*transpose(input, input);
	flip(input, input,1);
	transpose(input, input);
	flip(input, input, 1);*/
	imwrite("output.jpg", input);
	imshow("output", output);
	waitKey(1000);
}

//求出能量矩阵(Sobel梯度)
void SEAM_CARVING::get_energy(Mat& energy)
{
	//将图片转换为灰色
	Mat gray;
	cvtColor(input, gray, COLOR_BGR2GRAY);

	Mat x_gradiant;//x轴的梯度
	Mat y_gradiant;//y轴的梯度

	//Sobel算子
	Sobel(gray, x_gradiant, CV_64F, 1, 0);
	Sobel(gray, y_gradiant, CV_64F, 0, 1);

	Mat abs_x_gradiant;//x轴的梯度
	Mat abs_y_gradiant;//y轴的梯度

	//取绝对值
	convertScaleAbs(x_gradiant, abs_x_gradiant);
	convertScaleAbs(y_gradiant, abs_y_gradiant);

	//两个梯度进行相加/整合
	addWeighted(abs_x_gradiant, 0.5, abs_y_gradiant, 0.5, 0, energy);
	energy.convertTo(energy, CV_8UC1);

	namedWindow("grad", CV_WINDOW_NORMAL);
	imshow("grad", energy);
	waitKey(1);

	//imshow("x_gradiant.", x_gradiant);
	//imshow("y_gradiant", y_gradiant);
	//waitKey(3000);
	//imwrite("x_gradiant.jpg", x_gradiant);
	//imwrite("y_gradiant.jpg", y_gradiant);

	//两个梯度进行相加/整合
	// 
	
#if 0
	//Mat xy_gradiant(x_gradiant.size(), x_gradiant.type());
	energy = Mat(x_gradiant.size(), x_gradiant.type());
	int col = x_gradiant.cols;
	int row = y_gradiant.rows;

	for (int i = 0;i < row;i++)
	{
		for (int j = 0;j < col;j++)
		{
			int xy = x_gradiant.at<uchar>(i, j) + y_gradiant.at<uchar>(i, j);
			//xy_gradiant.at<uchar>(i, j) = saturate_cast<uchar>(xy);
			energy.at<uchar>(i, j) = saturate_cast<uchar>(xy);
			
		}
	}
	energy.convertTo(energy, CV_8UC1);
	namedWindow("xy", CV_WINDOW_NORMAL);
	imshow("xy", energy);
	waitKey(1);
	//imwrite("xy_gradiant.jpg", energy);
	
	
	//imwrite("converted_energy.jpg", energy);

#endif
}

//获取最小能量代价矩阵
//https://www.bilibili.com/video/BV1hA411L7Bp/?spm_id_from=333.880.my_history.page.click&vd_source=242881c2e0028b99f5965425eaeee832
void SEAM_CARVING::get_min_energy_cost(Mat& energy, vector<vector<Point>>& matrix)
{
	int row = energy.rows;
	int col = energy.cols;

	for (int i = 0;i < row;i++)
	{
		vector<Point> temp;

		for (int j = 0;j < col;j++)
		{
			//第一行 能力值不变
			if (i == 0)
			{
				float val = energy.at<uchar>(0, j);
				int path = -2;
				temp.push_back(Point(val, path));
			}

			else
			{
				//第一列 最小值只能由上方或右上方
				if (j == 0)
				{
					float val = energy.at<uchar>(i, j) + min(energy.at<uchar>(i - 1, j), energy.at<uchar>(i - 1, j + 1));
					int path = (energy.at<uchar>(i - 1, j) < energy.at<uchar>(i - 1, j + 1)) ? -1 : 0;
					temp.push_back(Point(val, path+1));
				}

				//最后一列 左上方或上方
				else if (j == col - 1)
				{
					float val = energy.at<uchar>(i, j) + min(energy.at<uchar>(i - 1, j - 1), energy.at<uchar>(i - 1, j));
					int path = (energy.at<uchar>(i - 1, j - 1) < energy.at<uchar>(i - 1, j)) ? -1 : 0;
					temp.push_back(Point(val, path));
				}

				else
				{
					float a = energy.at<uchar>(i - 1, j - 1);
					float b = energy.at<uchar>(i - 1, j);
					float c = energy.at<uchar>(i - 1, j + 1);

					float min_energy = min(min(a,b), c);
					float val = energy.at<uchar>(i, j) + min_energy;

					int path = -2;

#if 0
					float abc[3] = { a,b,c };
					float min = INT_MAX;
					for (int i = 0;i < 3;i++)
					{
						if (abc[i] < min)
						{
							min = abc[i];
							path = i-1;
						}
					}
#endif
#if 1
					if (min_energy == energy.at<uchar>(i - 1, j - 1))
						path = -1;
					if (min_energy == energy.at<uchar>(i - 1, j))
						path = 0;
					if (min_energy == energy.at<uchar>(i - 1, j + 1))
						path = 1;
#endif
					temp.push_back(Point(val, path));
				}
			}

		}
		matrix.push_back(temp);
	}
}

//找出最小能量路径
void SEAM_CARVING::get_seam(vector<vector<Point>>& matrix, vector<int>& seam, float& cost)
{
	//先求出矩阵最后一行中最小的元素
	int i = matrix.size() - 1;
	int min = INT_MAX;
	int pos = -1;

	for (int j = 0;j < matrix[i].size();j++)
	{
		if (matrix[i][j].val < min)
		{
			min = matrix[i][j].val;
			pos = j;
		}
	}

	cost = 0;
	seam.push_back(pos);
	cost += matrix[i][pos].val;

	//回溯找回最短路径
	while (i > 0)
	{
		pos = pos + matrix[i][pos].path;
		seam.push_back(pos + matrix[i][pos].path);
		cost += matrix[i][pos].val;
		i--;
	}

}

//在rgb图像上进行删除最小能量像素路径
void SEAM_CARVING::remove_seam(vector<int>& seam, Mat& des)
{
	//将最短路径标记为红色
	Mat temp = input.clone();
	for (int i = 0;i < seam.size();i++)
	{
		int j = seam[i];
		Vec3b values(0, 0, 255);
		temp.at<Vec3b>(i, j) = values;
	}

	namedWindow("seam", CV_WINDOW_NORMAL);
	imshow("seam", temp);
	waitKey(1);

	//进行移位操作
	for (int i = 0;i < seam.size();i++)
	{
		int j = seam[i];
		//路径前的保持不变
		for (int k = 0;k < j;k++)
			des.at<Vec3b>(i, k) = input.at<Vec3b>(i, k);

		//路径后的像素向左移位一位
		for (j;j < input.cols - 1;j++)
			des.at<Vec3b>(i, j) = input.at<Vec3b>(i, j + 1);
	}
}
