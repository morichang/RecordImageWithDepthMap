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
	HRESULT hResult;

	std::vector<DepthSpacePoint> depthSpace;
	std::vector<UINT16> buffer;

	cv::Size color;
	cv::Size depth;

	int colorW;
	int colorH;
	int depthW;
	int depthH;
	
	int frameCount;
	unsigned int colorBytesPerPixel;
	bool initFlag;

public:
	KinectPointManager(cv::Size color, cv::Size depth);
	
	void convKinectDataToImage(Kinect2Sensor &kinect, cv::Mat &color_res_depth); // Kinectで取得したデータをColor解像度のDepthMapにする
	void convImageResolutionToDepthSize(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &depth_res_image);
};