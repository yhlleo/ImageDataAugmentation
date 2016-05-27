
/*
   Image data augmentation: flip and rotate.

   - Editor: Yahui Liu.
   - Data:   2016-05-20
   - Email:  yahui.cvrs@gmail.com
   - Address: Computer Vision and Remote Sensing(CVRS), Lab.
*/

#ifndef DATA_AUGMENT_H
#define DATA_AUGMENT_H

#include <iostream>
#include <vector>

#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;

enum FlipModel{
	MIRROR_V,       // vertical
	MIRROR_H        // horizontal
};

enum RotateModel
{
	RAW_SIZE,        // raw size of the input image
//	TOTAL_RESERVE,   // TODO: total rotated image
	MAX_RECT_REGION  // max rectangle region without blank area
};

enum RadMeasure
{
	AU_ANG,         // angle
	AU_RAD          // radian
};

class DataAugment
{
public:
	DataAugment() 
	{
		isColBigger         = true;
		isColSimilarRow     = false;
	}
	~DataAugment() {}

public:
	void imageFlip( 
			cv::Mat& input, 
			cv::Mat& output,
			FlipModel fm  = MIRROR_H );

	void imageRotate(
			cv::Mat& input,
			cv::Mat& output,
			double   radian,
			RadMeasure radm = AU_RAD,
			RotateModel rm  = MAX_RECT_REGION );

	cv::Matx22d rotateMat( 
			double radian, 
			RadMeasure rm   = AU_RAD );

	std::vector<cv::Point2d> rotateVertex( 
			std::vector<cv::Point2d>& vertexs, 
			cv::Matx22d& rMat );

	cv::Size getImageRange( 
			std::vector<cv::Point2d>& vertexs );

	cv::Point2d getMaxRectRegion(
			cv::Point3d& line);

	cv::Point2d getCrossPoint( 
			std::vector<cv::Point2d>& vertexs/*,
			double angle,
			RadMeasure radm = AU_ANG*/ );

	cv::Point2d getSpecialCrossPoint( 
			std::vector<cv::Point2d>& vertexs );

	/* 
	    a(x1, y1) and b(x2, y2) are two vertexs of the image rectangle:
		  a ------ b
		  |        |
		  |        |
		  d ------ c

		Then, the line passed the two points is:
			(x1-x2)*y - (y1-y2)*x - y2*(x1-x2) + x2*(y1-y2) = 0  
		which can by represented as:
		    a*y + b*x + c = 0.
	*/
	cv::Point3d lineFunction(
			std::vector<cv::Point2d>& vertexs);

	cv::Point3d lineFunction( 
			cv::Point2d v1, 
			cv::Point2d v2 );

	double p2pDist( cv::Point2d p1, cv::Point2d p2 );

private: 
	bool isColBigger;
	bool isColSimilarRow;

};

#endif // DATA_AUGMENT_H