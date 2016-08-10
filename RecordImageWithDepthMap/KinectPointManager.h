#pragma once

#include <fstream>
#include <list>

#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>

#include <Kinect.h>
#include "../../../../Documents/Visual Studio 2013/Projects/Kinect2Wrapper/Kinect2Wrapper/Kinect2.h"

class KinectPointManager{
private:
	std::string filename;
	std::ofstream ofs;

	std::list<boost::posix_time::ptime> timeList;

	void flush();

	size_t maxBufferSize;
	size_t nowBuffer;

	boost::posix_time::ptime lastUpdate;
	double targetFPS;
public:
	void recordInit(std::string filename);
	void setData(); // どういう形でデータを保存するか…
	void recordClose();
	void convKinectDataToImage(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &depth); // Kinectで取得したデータをImageとDepth Mapに保存する?
	void depthmapRefinement(cv::Mat image, cv::Mat srcDepth); // Depth MapがボロボロなのでImageを利用して解像度を上げる
};