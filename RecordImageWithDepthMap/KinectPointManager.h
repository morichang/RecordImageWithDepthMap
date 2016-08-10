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
	void setData(); // �ǂ������`�Ńf�[�^��ۑ����邩�c
	void recordClose();
	void convKinectDataToImage(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &depth); // Kinect�Ŏ擾�����f�[�^��Image��Depth Map�ɕۑ�����?
	void depthmapRefinement(cv::Mat image, cv::Mat srcDepth); // Depth Map���{���{���Ȃ̂�Image�𗘗p���ĉ𑜓x���グ��
};