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
	std::list<cv::Mat> imageList;
	std::list<cv::Mat> depthList;

	void flush();

	size_t maxBufferSize;
	size_t nowBuffer;

	boost::posix_time::ptime lastUpdate;
	bool initFlag;
	int frameCount;
	double targetFPS;
	unsigned int colorBytesPerPixel;

public:
	KinectPointManager(cv::Size color, cv::Size depth);

	void recordInit(std::string filename);
	void recordClose();

	void setData(); // �ǂ������`�Ńf�[�^��ۑ����邩�cImage��PNG?Depth��PNG?�炾�����o���Ă��ꂾ��PNG�ő���JPG?
	
	void convKinectDataToImage(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &raw_depth, cv::Mat &high_depth, std::vector<UINT16> &depthBuffer); // Kinect�Ŏ擾�����f�[�^��Image��Depth Map�ɕۑ�����?
	void convImageResolutionToDepthSize(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &depth_res_image);
	void depthmapRefinement(cv::Mat image, cv::Mat srcDepth); // Depth Map���{���{���Ȃ̂�Image�𗘗p���ĉ𑜓x���グ��
};