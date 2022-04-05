#include <time.h> 
#include <numeric>
#include <fstream>
#include <iostream>
#include <vector>
#include <Eigen/Dense>
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
	myLine(cv::Point p1, cv::Point p2) 
	{
		if (p1.x != 0 && p2.x != 0)
		{
			float y1 = 1.0 * p1.y / p1.x;
			float y2 = 1.0 * p2.y / p2.x;
			float c1 = 1.0 / p1.x;
			float c2 = 1.0 / p2.x;
			b = (-(c2 - c1)) / (y2 - y1);
			a = (-1 - b * p1.y) / p1.x;
			c = 1;
		}
		else
		{
			printf("x of point can not be zero \n");
		}
	}
	myLine()
	{
		a = 0;
		b = 0;
		c = 0;
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
	void printABC()
	{
		printf("a=%f,b=%f,c=%f\n", a, b, c);
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
	const cv::Point wallLineOneRight = cv::Point(463, 291);
	const cv::Point wallLineOneLeft = cv::Point(97, 526);
	const cv::Point wallLineTwoRight = cv::Point(461, 552);
	const cv::Point wallLineTwoLeft = cv::Point(91, 707);

	//由两点确定直线
	myLine wallLineOne(wallLineOneLeft, wallLineOneRight);
	myLine wallLineTwo(wallLineTwoLeft, wallLineTwoRight);
	wallLineOne.printABC();
	wallLineTwo.printABC();
	//消影点
	const cv::Point crossPointAboutGrand = getCrossPoint(wallLineOne, wallLineTwo);
	printf("crossPointAboutGrand.x = %d, crossPointAboutGrand.y = %d \n", crossPointAboutGrand.x, crossPointAboutGrand.y);
	//画出选取的两条平行线
	cv::Mat showMat = gray.clone();
	cv::line(showMat, cv::Point(showMat.cols - 1, wallLineOne.getY(showMat.cols - 1)), cv::Point(0, wallLineOne.getY(0)), 255, 4, cv::LINE_AA);
	cv::line(showMat, cv::Point(showMat.cols - 1, wallLineTwo.getY(showMat.cols - 1)), cv::Point(0, wallLineTwo.getY(0)), 255, 4, cv::LINE_AA);
	cv::namedWindow("showMat", 0);
	cv::imshow("showMat", showMat);
	//cv::waitKey(0);

	//墙面两条直线的取点
	const cv::Point wallDiagonalLineOneRight = cv::Point(463, 291);
	const cv::Point wallDiagonalLineOneLeft = cv::Point(91, 707);
	const cv::Point wallDiagonalLineTwoRight = cv::Point(461, 552);
	const cv::Point wallDiagonalLineTwoLeft = cv::Point(89, 884);//使用（89，884）时看起来比较正常。取点取为（89，902）
	//由两点确定直线
	myLine wallDiagonalLineOne(wallDiagonalLineOneLeft, wallDiagonalLineOneRight);
	myLine wallDiagonalLineTwo(wallDiagonalLineTwoLeft, wallDiagonalLineTwoRight);
	//消影点
	const cv::Point crossPointAboutWall = getCrossPoint(wallDiagonalLineOne, wallDiagonalLineTwo);
	printf("crossPointAboutWall.x = %d, crossPointAboutWall.y = %d \n", crossPointAboutWall.x, crossPointAboutWall.y);
	//画出选取的两条平行线
	cv::line(showMat, cv::Point(showMat.cols - 1, wallDiagonalLineOne.getY(showMat.cols - 1)), cv::Point(0, wallDiagonalLineOne.getY(0)), 255, 4, cv::LINE_AA);
	cv::line(showMat, cv::Point(showMat.cols - 1, wallDiagonalLineTwo.getY(showMat.cols - 1)), cv::Point(0, wallDiagonalLineTwo.getY(0)), 255, 4, cv::LINE_AA);
	cv::imshow("showMat", showMat);
	
	//由两点确定消影线
	myLine fadingLine(crossPointAboutGrand, crossPointAboutWall);
	fadingLine.printABC();
	//画出消影线
	cv::line(showMat, crossPointAboutGrand, crossPointAboutWall, 255, 4, cv::LINE_AA);
	cv::imshow("showMat", showMat);
	//cv::waitKey(0);
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
	cv::Mat wrapDst = cv::Mat::zeros(gray.rows, gray.cols, gray.type());
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
	double temp[3][3] = { 1,0,0,0,1,0,fadingLine.a, fadingLine.b, fadingLine.c };
	cv::Mat warp_mat(3, 3, CV_64F,temp);
	warp_mat = warp_mat;
	printMat(warp_mat);
	warp_mat.convertTo(warp_mat, warpmatrix.type(), 1.0);
	cv::warpPerspective(wrapSrc, wrapDst, warp_mat, wrapDst.size(), cv::INTER_LINEAR);//投射变换

	cv::namedWindow("wrapDst", 0);
	cv::imshow("wrapDst", wrapDst);

	//cv::imwrite("wrapDstGood.jpg", wrapDst);

	/********************************************度量矫正***************************************************/
	cv::Mat wrapDstShow = wrapDst.clone();
	std::vector<std::pair<myLine, myLine>> VerticalLine(3);
	std::vector<std::pair<std::pair<cv::Point, cv::Point>, std::pair<cv::Point, cv::Point>>> VerticalLinePoint(3);

	//取3*2*2个点
	VerticalLinePoint[0].first.first = cv::Point(282, 245);
	VerticalLinePoint[0].first.second = cv::Point(282, 279);
	VerticalLinePoint[0].second.first = cv::Point(277, 244);
	VerticalLinePoint[0].second.second = cv::Point(261, 267);

	VerticalLinePoint[1].first.first = cv::Point(91, 692);
	VerticalLinePoint[1].first.second = cv::Point(91, 768);
	VerticalLinePoint[1].second.first = cv::Point(111, 765);
	VerticalLinePoint[1].second.second = cv::Point(95, 787);

	//VerticalLinePoint[2].first.first = cv::Point(88, 613);
	//VerticalLinePoint[2].first.second = cv::Point(272, 341);
	//VerticalLinePoint[2].second.first = cv::Point(174, 593);
	//VerticalLinePoint[2].second.second = cv::Point(174, 659);
	VerticalLinePoint[2].first.first = cv::Point(278, 393);
	VerticalLinePoint[2].first.second = cv::Point(263, 440);
	VerticalLinePoint[2].second.first = cv::Point(263, 414);
	VerticalLinePoint[2].second.second = cv::Point(278, 418);
	for (int i = 0; i < 3; i++)
	{
		VerticalLine[i].first = myLine(VerticalLinePoint[i].first.first, VerticalLinePoint[i].first.second);
		VerticalLine[i].first.printABC();
		VerticalLine[i].second = myLine(VerticalLinePoint[i].second.first, VerticalLinePoint[i].second.second);
		VerticalLine[i].second.printABC();

		cv::line(wrapDstShow, VerticalLinePoint[i].first.first, VerticalLinePoint[i].first.second, 255, 2, cv::LINE_AA);
		cv::line(wrapDstShow, VerticalLinePoint[i].second.first, VerticalLinePoint[i].second.second, 255, 2, cv::LINE_AA);
	}
	cv::namedWindow("wrapDstShow", 0);
	cv::imshow("wrapDstShow", wrapDstShow);

	//展开成行向量
	std::vector<std::vector<float>> VerticalLineForMatrix(3, std::vector<float>(3));
	for (int i = 0; i < 3; i++)
	{
		VerticalLineForMatrix[i][0] = VerticalLine[i].first.a * VerticalLine[i].second.a;
		VerticalLineForMatrix[i][1] = VerticalLine[i].first.a * VerticalLine[i].second.b + VerticalLine[i].first.b * VerticalLine[i].second.a;
		VerticalLineForMatrix[i][2] = VerticalLine[i].first.b * VerticalLine[i].second.b;

		printf("%f  %f   %f \n", VerticalLineForMatrix[i][0], VerticalLineForMatrix[i][1], VerticalLineForMatrix[i][2]);
	}

	Eigen::Matrix<double, 3, 3> C;
	C << VerticalLineForMatrix[1][0], VerticalLineForMatrix[1][1], VerticalLineForMatrix[1][2],
		VerticalLineForMatrix[2][0], VerticalLineForMatrix[2][1], VerticalLineForMatrix[2][2],
		0, 0, 0;
	C.eigenvalues();
	Eigen::EigenSolver<Eigen::Matrix3d> eig(C);
	std::cout << eig.eigenvectors() << std::endl;
	auto vec = eig.eigenvectors();
	std::vector<double> s(3);
	s[0] = vec(6).real();
	s[1] = vec(7).real();
	s[2] = vec(8).real();
	//for (int i = 0; i < vec.rows(); i++)
	//{
	//	std::cout << vec[i];
	//}
	//std::cout << std::endl;

	Eigen::Matrix3f S;
	S << s[0], s[1], 0,
		 s[1], s[2], 0,
		 0, 0, 0;
	Eigen::JacobiSVD<Eigen::MatrixXf> svd(S, Eigen::ComputeThinU | Eigen::ComputeThinV);
	Eigen::Matrix3f V = svd.matrixV(), U = svd.matrixU();
	Eigen::Matrix3f  SofSVD = U.inverse() * S * V.transpose().inverse();
	std::cout << "s of svd:" << SofSVD << std::endl;
	double Htemp[3][3] = { V(0,0),V(0,1),V(0,2),
						   V(1,0),V(1,1),V(1,2),
						   V(2,0),V(2,1),V(2,2)};
	cv::Mat H(3, 3, CV_64F, Htemp);
	H.convertTo(H, warpmatrix.type(), 1.0);
	printMat(H);
	cv::Mat metricCorrection;
	cv::warpPerspective(wrapDst, metricCorrection, H, wrapDst.size(), cv::INTER_LINEAR);//投射变换
	cv::namedWindow("metricCorrection", 0);
	cv::imshow("metricCorrection", metricCorrection);

	/********************************************仿射变换***************************************************/
	cv::Mat wrapSrFS = wrapDst.clone();
	cv::Mat wrapDstFS = cv::Mat::zeros(wrapSrFS.rows, 2* wrapSrFS.cols, wrapSrFS.type());

	cv::Point2f srcTriFS[3], dstTriFS[3];
	srcTriFS[0] = cv::Point2f(94, 513);
	srcTriFS[1] = cv::Point2f(93, 784);
	srcTriFS[2] = cv::Point2f(281, 238);
	int lengthFS = sqrt((srcTriFS[2].x - srcTriFS[0].x) * (srcTriFS[2].x - srcTriFS[0].x) + (srcTriFS[2].y - srcTriFS[0].y) * (srcTriFS[2].y - srcTriFS[0].y));

	dstTriFS[0] = cv::Point2f(wrapSrFS.cols / 2 - (wrapSrFS.cols / 2 - 75), 513);
	dstTriFS[1] = cv::Point2f(wrapSrFS.cols / 2 - (wrapSrFS.cols / 2 - 75), 784);
	dstTriFS[2] = cv::Point2f(dstTri[0].x+ lengthFS /2, 513);

	cv::Mat warp_matFS(2, 3, CV_32FC1);
	warp_matFS = cv::getAffineTransform(srcTriFS, dstTriFS);
	printMat(warp_matFS);

	//仿射变换
	warpAffine(wrapSrFS, wrapDstFS, warp_matFS, wrapDst.size());
	cv::namedWindow("wrapDstFS", 0);
	cv::imshow("wrapDstFS", wrapDstFS);




	cv::waitKey(0);
	return 0;
}
