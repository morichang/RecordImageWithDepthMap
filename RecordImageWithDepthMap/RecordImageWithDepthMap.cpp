// RecordImageWithDepthMap.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include "KinectPointManager.h"

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

int _tmain(int argc, _TCHAR* argv[])
{
	Kinect2Sensor kinect;

	if (useKinect){
		kinect.initialize();
	}

	cv::Size colorSize(kinect.getColorSize().width, kinect.getColorSize().height);
	cv::Size depthSize(kinect.getDepthSize().width, kinect.getDepthSize().height);

	KinectPointManager KPManager(colorSize, depthSize);

	// Key入力用
	cv::imshow("Key Capture", NULL);

	cv::Mat image = cv::Mat::zeros(colorSize, CV_8UC3);
	cv::Mat colorImage = cv::Mat::zeros(colorSize, CV_8UC3);
	cv::Mat harfImage = cv::Mat::zeros(colorSize.width/2,colorSize.height/2, CV_8UC3);
	cv::Mat raw_depth = cv::Mat::zeros(depthSize, CV_16UC1);
	cv::Mat high_depth = cv::Mat::zeros(colorSize, CV_16UC1);
	cv::Mat depth = cv::Mat::zeros(colorSize, CV_8UC1);

	while (kinect.update());

	std::cout << "Kinect起動します!" << std::endl;

	char key = 0;
	while (key != 'q'){
		if (useKinect){
			bool kinectUpdated = kinect.update();

			if (kinectUpdated){
				KPManager.convKinectDataToImage(kinect, image, raw_depth, high_depth);
				colorImage = kinect.getFrame();
				
				//std::cout << "撮れてる" << std::endl;
			}
			
			if (!image.empty() && !raw_depth.empty()) {
				// 画像確認用
				//cv::resize(image, harfImage, cv::Size(), 0.5, 0.5);
				//cv::imshow("Color Image", harfImage);
				high_depth.convertTo(depth, CV_8UC1, -255.0f / 8000.0f, 255.0f);
				cv::imshow("Raw Depth Image", raw_depth);
				cv::imshow("Color Resolution Depth Image", high_depth);
				cv::imshow("Depth Image", depth);
				if (key == 's'){
					cv::imwrite("color_image.png", image);
					cv::imwrite("raw_depth.png", raw_depth);
					cv::imwrite("depth_image.png", depth);
				}
				image.release();
				colorImage.release();
				raw_depth.release();
			}
		}

		key = cv::waitKey(1); //ウィンドウの更新
	}

	return 0;
}

