// RecordImageWithDepthMap.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include "KinectPointManager.h"
#include "KinectAudioManager.h"
#include "BackgroundSubtraction.h"
#include "DepthDenoising.h"
#include "BinaryMatIO.h"

#include "../../Kinect2Wrapper/Kinect2Wrapper/Kinect2.h"
//#include "../../../../Documents/Visual Studio 2013/Projects/Kinect2Wrapper/Kinect2Wrapper/Kinect2.h"

#ifdef _DEBUG
#pragma comment(lib,"../../Kinect2Wrapper/Debug/Kinect2Wrapper.lib")
//#pragma comment(lib, "../../../../Documents/Visual Studio 2013/Projects/Kinect2Wrapper/Debug/Kinect2Wrapper.lib")
#else
#pragma comment(lib,"../../Kinect2Wrapper/Release/Kinect2Wrapper.lib")
//#pragma comment(lib,"../../../../Documents/Visual Studio 2013/Projects/Kinect2Wrapper/Release/Kinect2Wrapper.lib")
#endif

const static bool useKinect = true;
HRESULT hResult = S_OK;

int _tmain(int argc, _TCHAR* argv[])
{
	Kinect2Sensor kinect;

	if (useKinect){
		kinect.initialize();
	}

	cv::Size colorSize(kinect.getColorSize().width, kinect.getColorSize().height);
	cv::Size depthSize(kinect.getDepthSize().width, kinect.getDepthSize().height);

	KinectPointManager KPManager(colorSize, depthSize);
	KinectAudioManager KAManager;
	BinaryMatIO recorder = BinaryMatIO();
	//BackgroundSubtraction back(colorSize);

	// Key入力用
	//cv::imshow("Key Capture", NULL);

	cv::Mat image = cv::Mat(colorSize, CV_8UC3);
	cv::Mat depth = cv::Mat(colorSize, CV_16UC1);
	//cv::Mat harfImage = cv::Mat(colorSize.width/2,colorSize.height/2, CV_8UC3);

	boost::posix_time::ptime nowTime;
	boost::posix_time::ptime lastUpdate;

	while (kinect.update());

	std::cout << "Kinect起動します!" << std::endl;

	//KAManager.audioInitialize(kinect);

	bool kinectUpdated = false;
	
	boost::posix_time::ptime recordStart = boost::posix_time::microsec_clock::local_time();
	std::ostringstream oss;
	auto facet = new boost::posix_time::time_facet("%Y-%m-%d_%H-%M-%S");  // 2017-01-09_03:24:00
	oss.imbue(std::locale(oss.getloc(), facet));
	oss << recordStart;

	recorder.recordInitialize(oss.str() + "_" + recorder.computerName + ".bin", true);

	double totalMinutes = 0;
	const double recordMinutes = 3 * 60 * 1000;  // 3[m] * 60[s] * 1000[ms]

	lastUpdate = recordStart;

	char key = 0;
	while (totalMinutes < recordMinutes){
		if (useKinect){
			nowTime = boost::posix_time::microsec_clock::local_time();
			kinectUpdated = kinect.update();

			if (kinectUpdated){
				// Imageに現フレームの画像を入れる
				image = kinect.getFrame();
				KPManager.convKinectDataToImage(kinect, depth);
				recorder.setData(image, depth);
				//KAManager.audioUpdate();
				//back.updateImageAndDepth(src_image, image, split_image, src_depth, high_depth, split_depth);
			}

			if (!image.empty() && !depth.empty()) {
				// 画像確認用
				//cv::resize(image, harfImage, cv::Size(), 0.5, 0.5);
				//cv::imshow("Color Image", harfImage);
				//cv::imshow("Color Resolution Depth Image", depth);
				image.release();
				depth.release();
			}

			totalMinutes += (nowTime - lastUpdate).total_milliseconds();
			lastUpdate = nowTime;
		}
		//recorder.readData();
		//key = cv::waitKey(1); //ウィンドウの更新
	}

	KAManager.audioClose();
	recorder.recordClose();

	return 0;
}