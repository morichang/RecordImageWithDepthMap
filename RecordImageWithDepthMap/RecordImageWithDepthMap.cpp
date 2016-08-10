// RecordImageWithDepthMap.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include "KinectPointManager.h"

#include "../../../../Documents/Visual Studio 2013/Projects/Kinect2Wrapper/Kinect2Wrapper/Kinect2.h"

#ifdef _DEBUG
#pragma comment(lib, "../../../../Documents/Visual Studio 2013/Projects/Kinect2Wrapper/Debug/Kinect2Wrapper.lib")
#else
#pragma comment(lib,"../../../../Documents/Visual Studio 2013/Projects/Kinect2Wrapper/Release/Kinect2Wrapper.lib")
#endif

const static bool useKinect = true;

int _tmain(int argc, _TCHAR* argv[])
{
	Kinect2Sensor kinect;
	KinectPointManager KPManager;

	if (useKinect){
		kinect.initialize();
	}

	cv::Mat image(cv::Size(1920, 1080), CV_8UC3);
	cv::Mat rawDepth(cv::Size(kinect.getDepthSize().width, kinect.getDepthSize().height), CV_16UC1);
	cv::Mat depth(cv::Size(1920, 1080), CV_8UC1);

	char key = 0;
	while (key != 'q'){
		if (useKinect){
			bool kinectUpdated = kinect.update();

			if (kinectUpdated){
				KPManager.convKinectDataToImage(kinect, image, rawDepth);
			}
		}
	}

	return 0;
}

