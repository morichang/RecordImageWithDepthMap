//#pragma once

#include <fstream>
#include <list>

#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>

#include <Kinect.h>
#include "../../Kinect2Wrapper/Kinect2Wrapper/Kinect2.h"
//#include "../../../../Documents/Visual Studio 2013/Projects/Kinect2Wrapper/Kinect2Wrapper/Kinect2.h"

class KinectPointManager{
private:
	cv::Size color;
	cv::Size depth;

	std::string filename;
	std::ofstream ofs;

	std::list<boost::posix_time::ptime> timeList;

	void flush();

	size_t maxBufferSize;
	size_t nowBuffer;

	boost::posix_time::ptime lastUpdate;
	double targetFPS;
	int colorBytesPerPixel;

public:
	KinectPointManager(cv::Size color, cv::Size depth);

	void recordInit(std::string filename);
	void recordClose();

	void setData(); // どういう形でデータを保存するか…ImageはPNG?DepthもPNG?顔だけ検出してそれだけPNGで他をJPG?
	
	void convKinectDataToImage(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &raw_depth, cv::Mat &high_depth); // Kinectで取得したデータをImageとDepth Mapに保存する?
	void depthmapRefinement(cv::Mat image, cv::Mat srcDepth); // Depth MapがボロボロなのでImageを利用して解像度を上げる
};