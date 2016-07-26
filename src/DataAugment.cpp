#include "DataAugment.h"

void DataAugment::imageFlip( 
	cv::Mat& input, 
	cv::Mat& output, 
	FlipModel fm /* = MIRROR_H */ )
{
	if ( input.channels() == 3 || input.channels() == 1)
	{
		const int rows = input.rows;
		const int cols = input.cols;

		cv::Mat(
			rows, 
			cols, 
			CV_MAKETYPE(CV_8U, input.channels()), 
			cv::Scalar::all(0)).copyTo(output);

		if ( fm == MIRROR_H )
		{
			switch ( input.channels() )
			{
			case 3:
				cv::Vec3b *ip3, *op3;

				for ( int i=0; i<rows; i++ )
				{
					ip3  = input.ptr<cv::Vec3b>(i);
					op3 = output.ptr<cv::Vec3b>(i);

					for ( int j=0; j<cols; j++ )
						op3[j] = ip3[cols - 1 - j];
				}
				break;
			case 1:
				uchar *ip1, *op1;

				for ( int i=0; i<rows; i++ )
				{
					ip1 = input.ptr<uchar>(i);
					op1 = output.ptr<uchar>(i);

					for ( int j=0; j<cols; j++ )
						op1[j] = ip1[cols - 1 - j];
				}
				break;
			default:
				break;
			}
		}
		else if ( fm == MIRROR_V )
		{
			for ( int i=0; i<rows; i++ )
				input.row(rows - i - 1).copyTo(output.row(i));
		}
		else
		{
			cout << "Unknown model of mirror" << endl;
		}
	}
	else
	{
		cout << "Image channels != 3 or 1, please check the input!" << endl;
	}
}

void DataAugment::imageRotate( 
	cv::Mat& input, 
	cv::Mat& output, 
	double radian, 
	RadMeasure radm /*= AU_RAD*/,
	RotateModel rm /* = MAX_RECT_REGION */ )
{
	const int cols = input.cols;
	const int rows = input.rows;

	if ( cols < rows)
	{
		isColBigger = false;
	}

	double theta = radian;
	if ( radm == AU_RAD )
		theta *= ( 180.0/CV_PI );
		
	if ( theta < 0.0 )
		theta += 360.0;

	cv::Mat R = cv::getRotationMatrix2D(
		cv::Point2f(
			static_cast<float>(cols/2), 
			static_cast<float>(rows/2)), 
		-theta, 
		1.0);

	cv::Mat r_input;
	cv::warpAffine( 
		input, 
		r_input, 
		R, 
		input.size());

	if ( rm == MAX_RECT_REGION )
	{
		theta -= int(theta / 180.0) * 180.0;

		std::vector<cv::Point2d> vertex(4);
		vertex[0] = cv::Point2d(-(cols-1)/2, (rows-1)/2);
		vertex[1] = cv::Point2d((cols-1)/2, (rows-1)/2);
		vertex[2] = cv::Point2d((cols-1)/2, -(rows-1)/2);
		vertex[3] = cv::Point2d(-(cols-1)/2,-(rows-1)/2);


		if ( theta > 0.0 && theta < 90.0 )
		{
			cv::Matx22d rMat = rotateMat(theta, AU_ANG);
			std::vector<cv::Point2d> r_vertext = 
				rotateVertex( vertex, rMat );

			cv::Point2d maxp = getCrossPoint(r_vertext/*, theta, AU_ANG*/);

			if ( maxp.x > cols/2 || maxp.y > rows/2 )
			{
				maxp = getSpecialCrossPoint(r_vertext);
			}

			maxp.x = maxp.x < cols/2 ? maxp.x : cols/2;
			maxp.y = maxp.y < rows/2 ? maxp.y : rows/2;

			cv::Rect rect(
				cols/2-maxp.x, 
				rows/2-maxp.y, 
				2*std::abs(maxp.x), 
				2*std::abs(maxp.y));

			r_input(rect).copyTo(output);
		}
		else if ( theta == 90.0 )
		{
			cv::Rect rect;
			if( cols > rows)
				rect = cv::Rect((cols-rows)/2, 0, rows, rows );
			else
				rect = cv::Rect(0, (rows-cols)/2, cols, cols );

			r_input(rect).copyTo(output);
		}
		else if ( theta > 90.0 )
		{
			double theta2 = 180.0 - theta;
			cv::Matx22d rMat = rotateMat(theta2, AU_ANG);
			std::vector<cv::Point2d> r_vertext = 
				rotateVertex( vertex, rMat );

			cv::Point2d maxp = getCrossPoint(r_vertext/*, theta2, AU_ANG*/);

			if ( maxp.x > cols/2 || maxp.y > rows/2 )
			{
				maxp = getSpecialCrossPoint(r_vertext);
			}

 			maxp.x = maxp.x < cols/2 ? maxp.x : cols/2;
 			maxp.y = maxp.y < rows/2 ? maxp.y : rows/2;

			cv::Rect rect(
				cols/2-maxp.x, 
				rows/2-maxp.y, 
				2*std::abs(maxp.x), 
				2*std::abs(maxp.y));

			r_input(rect).copyTo(output);
		}
		else
		{
			r_input.copyTo(output);
		}
	}
	else
	{
		r_input.copyTo(output);
	}
}

cv::Point2d DataAugment::getCrossPoint(
	std::vector<cv::Point2d>& vertexs)
{
// 	cv::Matx22d rMat = rotateMat(angle, radm);
// 	std::vector<cv::Point2d> r_vertext = 
// 		rotateVertex( vertexs, rMat );

	cv::Point3d ln_ab  = lineFunction(vertexs);
	return getMaxRectRegion(ln_ab);
}

cv::Point2d DataAugment::getSpecialCrossPoint( 
	std::vector<cv::Point2d>& vertexs )
{
	cv::Point3d line0_1 = lineFunction(vertexs[0], vertexs[1]);
	cv::Point3d line1_2 = lineFunction(vertexs[1], vertexs[2]);

	double a1			= line0_1.x;
	double b1			= line0_1.y;
	double c1			= line0_1.z;

	double a2			= line1_2.x;
	double b2			= line1_2.y;
	double c2			= line1_2.z;

	double x			= -(a1*c2 + a2*c1) / (a2*b1 + a1*b2);
	double y			= -(b1*x+c1) / a1;

	return cv::Point2d( x, y );
}

cv::Matx22d DataAugment::rotateMat( 
	double radian, 
	RadMeasure rm/* = AU_RAD*/ )
{
	double alpha = radian;

	if ( rm == AU_ANG )
		alpha *= CV_PI / 180.0;

	return cv::Matx22d( 
		std::cos(alpha),  std::sin(alpha), 
		-std::sin(alpha), std::cos(alpha) );
}

std::vector<cv::Point2d> DataAugment::rotateVertex( 
	std::vector<cv::Point2d>& vertexs, 
	cv::Matx22d& rMat )
{
	std::vector<cv::Point2d> rt = vertexs;
	const int v_size = vertexs.size();

	for ( int i=0; i<v_size; i++ )
	{
		cv::Matx21d v_i(
			vertexs[i].x, 
			vertexs[i].y), 
			v_r(0.0, 0.0);

		v_r   = rMat * v_i;
		
		rt[i] = cv::Point2d(v_r(0), v_r(1));
	}

	return rt;
}

double DataAugment::p2pDist( 
	cv::Point2d p1, 
	cv::Point2d p2 )
{
	return std::sqrt( ( p1.x - p2.x ) * ( p1.x - p2.x ) + 
		( p1.y - p2.y ) * ( p1.y - p2.y ) );
}


cv::Point3d DataAugment::lineFunction(
	std::vector<cv::Point2d>& vertexs)
{
	cv::Point2d pa = vertexs[0];
	cv::Point2d pb = vertexs[1];

	if ( !isColBigger )
	{
		pb = vertexs[3];
	}

	double delta_x = pa.x - pb.x;
	double delta_y = pa.y - pb.y;
	cv::Point3d line(delta_x, -delta_y, -pb.y*delta_x + pb.x*delta_y);

	double m_line = std::sqrt(line.x*line.x + line.y*line.y);
	line *= 1.0/m_line;

	if ( line.x < 0.0 )
		line = -line;

	return line;
}


cv::Point3d DataAugment::lineFunction( 
	cv::Point2d v1, 
	cv::Point2d v2 )
{
	cv::Point2d pa = v1;
	cv::Point2d pb = v2;

	double delta_x = pa.x - pb.x;
	double delta_y = pa.y - pb.y;
	cv::Point3d line(delta_x, -delta_y, -pb.y*delta_x + pb.x*delta_y);

	double m_line = std::sqrt(line.x*line.x + line.y*line.y);
	line *= 1.0/m_line;

	if ( line.x < 0.0 )
		line = -line;

	return line;
}

cv::Point2d DataAugment::getMaxRectRegion(
	cv::Point3d& line)
{
	if ( line.x != 0.0 && line.y != 0.0 )
	{
// 		double k = - line.y / line.x;   // k = -b/a
// 		double s = - line.z / line.x;   // s = -c/a
		double a = line.x;
		double b = line.y;
		double c = line.z;

		if ( !isColBigger )
		{
			b *= -1.0;
		}
		return cv::Point2d( -c/(2*b) , -c/(2*a) );
	}
	else
		return cv::Point2d(0.0, 0.0);
}

cv::Size DataAugment::getImageRange(
	std::vector<cv::Point2d>& vertexs )
{
	const int v_size = vertexs.size();
	cv::Point2d pMin(0.0, 0.0), pMax(0.0, 0.0);
	for ( int i=0; i<v_size; i++ )
	{
		pMin.x = pMin.x < vertexs[i].x ? pMin.x : vertexs[i].x;
		pMin.y = pMin.y < vertexs[i].y ? pMin.y : vertexs[i].y;
		pMax.x = pMax.x > vertexs[i].x ? pMax.x : vertexs[i].x;
		pMax.y = pMax.y > vertexs[i].y ? pMax.y : vertexs[i].y;
	}

	return cv::Size(
		static_cast<int>(pMax.x - pMin.x + 1.0), 
		static_cast<int>(pMax.y - pMin.y + 1.0));
}
