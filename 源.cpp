#include <time.h> 
#include <numeric>
#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

class myLine
{
public:
	myLine(float aa, float bb, float cc):a(aa), b(bb), c(cc)
	{
		if (aa == 0 && bb == 0)
		{
			printf("line is wrong \n");
		}

	}

	~myLine()
	{
	}

	int getX(int y) const
	{
		int x = 0;
		if (a != 0)
		{
			x = static_cast<int>((0 - c - b * y) / a);
		}
		else
		{
			printf("x can be any value \n");
		}
		return x;
	}
	int getY(int x) const
	{
		int y;
		if (b != 0)
		{
			y = static_cast<int>((0 - c - a * x) / b);
		}
		else
		{
			printf("y can be any value \n");
		}
		return y;
	}

	friend cv::Point getCrossPoint(myLine lineOne, myLine lineTwo) 
	{
		cv::Point cross;
		if (lineOne.a != 0 && lineOne.b != 0 && lineTwo.a != 0 && lineTwo.b != 0)//正常情况
		{
			float b1 = lineOne.b / lineOne.a;
			float c1 = lineOne.c / lineOne.a;

			float b2 = lineTwo.b / lineTwo.a;
			float c2 = lineTwo.c / lineTwo.a;

			cross.y = static_cast<int>(-(c2 - c1) / (b2 - b1));
			cross.x = lineOne.getX(cross.y);
		}
		//特殊处理水平线
		if (lineOne.a == 0 &&lineTwo.a != 0)
		{
			cross.y = lineOne.getY(0);
			cross.x = lineTwo.getX(cross.y);
		}
		else if (lineOne.a != 0 && lineTwo.a == 0)
		{
			cross.y = lineTwo.getY(0);
			cross.x = lineOne.getX(cross.y);
		}
		
		//特殊处理竖直线
		if (lineOne.b == 0 && lineTwo.b != 0)
		{
			cross.x = lineOne.getX(0);
			cross.y = lineTwo.getY(cross.x);
		}
		else if (lineOne.b != 0 && lineTwo.b == 0)
		{
			cross.x = lineTwo.getX(0);
			cross.y = lineOne.getY(cross.x);
		}

		return cross;

	}

private:
	float a, b, c;
};
void printMat(cv::Mat A)
{
	using namespace std;
	for (int i = 0; i < A.rows; i++)
	{
		for (int j = 0; j < A.cols; j++)
		{
			cout << A.at<double>(i, j) << ' ';
		}
		cout << endl;
	}
	cout << endl;
}



int main()
{
	cv::Mat src, gray;
	src = cv::imread("src.jpg");
	if (src.channels() != 1)
		cv::cvtColor(src, gray, CV_BGR2GRAY);
	else
		gray = src.clone();

	//cv::GaussianBlur(gray, gray, cv::Size(3, 3), cv::BORDER_DEFAULT);
	cv::namedWindow("gray", 0);
	cv::imshow("gray", gray);
	//cv::waitKey(0);

	//cv::Mat cannyGray;
	//cv::Canny(gray, cannyGray, 200, 100, 3, false);
	//cv::namedWindow("cannyGray", 0);
	//cv::imshow("cannyGray", cannyGray);
	//cv::imwrite("cannyGray.jpg", cannyGray);
	//cv::waitKey(0);
	/*********************************************计算消影点和消影线***************************************************/
	//地面两条直线的取点
	const cv::Point grandLineOneRight = cv::Point(609, 1144);
	const cv::Point grandLineOneLeft = cv::Point(62, 1108);
	const cv::Point grandLineTwoRight = cv::Point(467, 1184);
	const cv::Point grandLineTwoLeft = cv::Point(43, 1137);

	//由两点确定直线
	myLine grandLineOne(36, -547, 603844);
	myLine grandLineTwo(47, -424, 480067);
	//消影点
	const cv::Point crossPointAboutGrand = getCrossPoint(grandLineOne, grandLineTwo);
	printf("crossPointAboutGrand.x = %d, crossPointAboutGrand.y = %d \n", crossPointAboutGrand.x, crossPointAboutGrand.y);
	//画出选取的两条平行线
	cv::Mat showMat = gray.clone();
	cv::line(showMat, cv::Point(showMat.cols - 1, grandLineOne.getY(showMat.cols - 1)), cv::Point(0, grandLineOne.getY(0)), 255, 4, cv::LINE_AA);
	cv::line(showMat, cv::Point(showMat.cols - 1, grandLineTwo.getY(showMat.cols - 1)), cv::Point(0, grandLineTwo.getY(0)), 255, 4, cv::LINE_AA);
	cv::namedWindow("showMat", 0);
	cv::imshow("showMat", showMat);
	//cv::waitKey(0);

	//墙面两条直线的取点
	const cv::Point wallLineOneRight = cv::Point(535, 839);
	const cv::Point wallLineOneLeft = cv::Point(472, 816);
	const cv::Point wallLineTwoRight = cv::Point(535, 634);
	const cv::Point wallLineTwoLeft = cv::Point(469, 584);
	//由两点确定直线
	myLine wallLineOne(23, -63, 40522);
	myLine wallLineTwo(50, -66, 15094);
	//消影点
	const cv::Point crossPointAboutWall = getCrossPoint(wallLineOne, wallLineTwo);
	printf("crossPointAboutWall.x = %d, crossPointAboutWall.y = %d \n", crossPointAboutWall.x, crossPointAboutWall.y);
	//画出选取的两条平行线
	cv::line(showMat, cv::Point(showMat.cols - 1, wallLineOne.getY(showMat.cols - 1)), cv::Point(0, wallLineOne.getY(0)), 255, 4, cv::LINE_AA);
	cv::line(showMat, cv::Point(showMat.cols - 1, wallLineTwo.getY(showMat.cols - 1)), cv::Point(0, wallLineTwo.getY(0)), 255, 4, cv::LINE_AA);
	cv::imshow("showMat", showMat);
	
	//由两点确定消影线
	myLine fadingLine(-34, -1690, 1773156);
	//画出消影线
	cv::line(showMat, crossPointAboutGrand, crossPointAboutWall, 255, 4, cv::LINE_AA);
	cv::imshow("showMat", showMat);
	
	/********************************************仿射变换***************************************************/
	//cv::Mat wrapSrc = gray.clone();
	//cv::Mat wrapDst = cv::Mat::zeros(gray.rows, 2*gray.cols, gray.type());

	//cv::Point2f srcTri[3], dstTri[3];
	//srcTri[0] = cv::Point2f(75, 695);
	//srcTri[1] = cv::Point2f(75, 897);
	//srcTri[2] = cv::Point2f(462, 524);
	//int length = sqrt((462 - 75) * (462 - 75) + (524 - 695) * (524 - 695));

	//dstTri[0] = cv::Point2f(wrapDst.cols / 2 - (gray.cols / 2 - 75), 695);
	//dstTri[1] = cv::Point2f(wrapDst.cols / 2 - (gray.cols / 2 - 75), 897);
	//dstTri[2] = cv::Point2f(dstTri[0].x+length/2, 695);

	//cv::Mat warp_mat(2, 3, CV_32FC1);
	//double temp[3][3] = { 1,0,0,0,1,0,-34, -1690, 1773156 };
	//cv::Mat warp_mat_multiplier(3, 3, CV_64F,temp);
	//warp_mat = cv::getAffineTransform(srcTri, dstTri);
	//warp_mat.convertTo(warp_mat, CV_64F, 1.0);
	////printMat(warp_mat);
	////warp_mat = warp_mat * warp_mat_multiplier;
	////printMat(warp_mat);
	////cv::invert(warp_mat, warp_mat);
	////printMat(warp_mat);
	//warp_mat.convertTo(warp_mat, CV_32FC1, 1.0);
	//warp_mat_multiplier.convertTo(warp_mat_multiplier, CV_32FC1, 1.0);
	////仿射变换
	//warpAffine(wrapSrc, wrapDst, warp_mat_multiplier, wrapDst.size());
	//cv::namedWindow("wrapDst", 0);
	//cv::imshow("wrapDst", wrapDst);
	/********************************************射影变换***************************************************/
	cv::Mat wrapSrc = gray.clone();
	cv::Mat wrapDst = cv::Mat::zeros(4*gray.rows, 4*gray.cols, gray.type());
	cv::Point2f srcTri[4], dstTri[4];
	srcTri[0] = cv::Point2f(75, 695);
	srcTri[1] = cv::Point2f(75, 897);
	srcTri[2] = cv::Point2f(462, 524);
	srcTri[3] = cv::Point2f(462, 824);
	int length = sqrt((462 - 75) * (462 - 75) + (524 - 695) * (524 - 695));

	dstTri[0] = cv::Point2f(wrapDst.cols / 2 - (gray.cols / 2 - 75), 695);
	dstTri[1] = cv::Point2f(wrapDst.cols / 2 - (gray.cols / 2 - 75), 897);
	dstTri[2] = cv::Point2f(dstTri[0].x + length, 695);
	dstTri[3] = cv::Point2f(dstTri[0].x + length, 897);
	cv::Mat warpmatrix = cv::getPerspectiveTransform(srcTri, dstTri);
	printMat(warpmatrix);
	double temp[3][3] = { 1,0,0,0,1,0,-34.0/ 1773156, -1690.0/ 1773156, 1 };
	cv::Mat warp_mat(3, 3, CV_64F,temp);
	printMat(warp_mat);
	warp_mat.convertTo(warp_mat, warpmatrix.type(), 1.0);
	cv::warpPerspective(wrapSrc, wrapDst, warp_mat, wrapDst.size(), cv::INTER_LINEAR);//投射变换

	cv::namedWindow("wrapDst", 0);
	cv::imshow("wrapDst", wrapDst);
	cv::waitKey(0);



	return 0;
}
