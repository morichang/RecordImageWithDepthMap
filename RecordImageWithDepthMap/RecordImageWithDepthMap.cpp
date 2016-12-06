// RecordImageWithDepthMap.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include "KinectPointManager.h"
#include "BackgroundSubtraction.h"
#include "DepthDenoising.h"

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
	CameraIntrinsics cameraIntrinsics;
	std::ofstream outputFile("cameraIntrinsics.txt");

	KinectPointManager KPManager(colorSize, depthSize);
	BackgroundSubtraction back(colorSize);

	// Key入力用
	cv::imshow("Key Capture", NULL);

	cv::Mat image = cv::Mat::zeros(colorSize, CV_8UC3);
	cv::Mat color = cv::Mat::zeros(colorSize, CV_8UC3);
	cv::Mat src_image = cv::Mat::zeros(colorSize, CV_8UC3);
	cv::Mat depth_res_image = cv::Mat::zeros(depthSize, CV_8UC3);
	cv::Mat harfImage = cv::Mat::zeros(colorSize.width/2,colorSize.height/2, CV_8UC3);
	cv::Mat raw_depth = cv::Mat::zeros(depthSize, CV_16UC1);
	cv::Mat high_depth = cv::Mat::zeros(colorSize, CV_16UC1);
	cv::Mat src_depth;
	cv::Mat depth;
	cv::Mat split_image = cv::Mat::zeros(colorSize, CV_8UC3);
	cv::Mat split_depth;
	std::vector<UINT16> depthBuffer;
	std::vector<cv::Mat> rawDepth_array;

	while (kinect.update());

	std::cout << "Kinect起動します!" << std::endl;

	char key = 0;
	int count = 0;
	while (key != 'q'){
		if (useKinect){
			bool kinectUpdated = kinect.update();

			if (kinectUpdated){
				hResult = kinect.getCoordinateMapper()->GetDepthCameraIntrinsics(&cameraIntrinsics);
				if (FAILED(hResult)) {
					std::cerr << "Error : ICoordinateMapper::GetDepthCameraIntrinsics()" << std::endl;
				}
				outputFile << "FocalLength" << cameraIntrinsics.FocalLengthX << "," << cameraIntrinsics.FocalLengthY << "\n";
				outputFile << "PrincipalPoint" << cameraIntrinsics.PrincipalPointX << "," << cameraIntrinsics.PrincipalPointY << "\n";
				outputFile << "RadialDistortionFourthOrder" << cameraIntrinsics.RadialDistortionFourthOrder << "\n";
				outputFile << "RadialDistortionSecondOrder" << cameraIntrinsics.RadialDistortionSecondOrder << "\n";
				outputFile << "RadialDistortionSixthOrder" << cameraIntrinsics.RadialDistortionSixthOrder << "\n";
				outputFile.flush();
				outputFile.close();
				//src_image = kinect.getFrame();
				KPManager.convKinectDataToImage(kinect, image, raw_depth, high_depth, depthBuffer);
				KPManager.convImageResolutionToDepthSize(kinect, src_image, depth_res_image);
				//back.updateImageAndDepth(src_image, image, split_image, src_depth, high_depth, split_depth);

				//std::cout << "撮れてる" << std::endl;
				if (key == 'c'){
					image.copyTo(src_image);
				   high_depth.copyTo(src_depth);
				}
			}
			
			if (!image.empty()) {
				// 画像確認用
				//cv::resize(image, harfImage, cv::Size(), 0.5, 0.5);
				//cv::imshow("Color Image", harfImage);
				//cv::resize(src_image, split_image, cv::Size(), 0.5, 0.5);
				//raw_depth.convertTo(depth, CV_8UC1, -255.0f / 8000.0f, 255.0f);
				//cv::imshow("Depth Resolution Color Image", depth_res_image);
				//cv::imshow("Raw Depth Image", raw_depth);
				//scv::imshow("Color Resolution Depth Image", high_depth);
				//cv::imshow("Depth Image", depth);
				//cv::imshow("Test1", src_image);
				//cv::imshow("Test2", split_image);
				//cv::imwrite("Test.png", split_image);
				cv::imwrite("color" + std::to_string(count) + ".png", depth_res_image);
				cv::imwrite("depth" + std::to_string(count) + ".png", raw_depth);
				if (key == 's'){
					cv::imwrite("depth.png", raw_depth);
					//cv::imwrite("color_image.png", image);
					//cv::imwrite("raw_depth.png", high_depth);
					//cv::imwrite("depth_image.png", depth);
					//cv::imwrite("backgroundSubtraction_image.png", split_image);
					//cv::imwrite("backgroundSubtraction_depth.png", split_depth);
					//cv::imwrite("test_depth_res_image.png", depth_res_image);
				}
				image.release();
				//src_image.release();
				raw_depth.release();
				count++;
				if (count > 60){
					break;
				}
			}
		}
		/*if (key == 'r'){
			std::string srcImage = "color_image.png";
			std::string srcDepth = "raw_depth.png";
			cv::Mat readImage = cv::imread(srcImage, 1);
			cv::Mat readDepth = cv::imread(srcDepth, -1);
			cv::Mat resizeImage = cv::Mat::zeros(1080, 1440, CV_8UC3);
			cv::Mat resizeDepth = cv::Mat::zeros(1080, 1440, CV_16UC1);
			if (readDepth.type() == CV_16UC1 && readImage.type() == CV_8UC3){
				std::cout << "Type合ってる!:" << readImage.type() << "," << readDepth.type() << std::endl;
				std::cout << readDepth.size().width << "," << readDepth.size().height << std::endl;
			}
			for (int y = 0; y < readImage.rows; y++){
				for (int x = 0; x < 1440; x++){
					resizeImage.at<cv::Vec3b>(y, x)[0] = readImage.at<cv::Vec3b>(y, x + 240)[0];
					resizeImage.at<cv::Vec3b>(y, x)[1] = readImage.at<cv::Vec3b>(y, x + 240)[1];
					resizeImage.at<cv::Vec3b>(y, x)[2] = readImage.at<cv::Vec3b>(y, x + 240)[2];
				}
			}
			cv::imshow("aa", resizeImage);
			cv::imwrite("resize_image.png", resizeImage);
			for (int y = 0; y < readDepth.rows; y++){
				for (int x = 0; x < 1440; x++){
					resizeDepth.at<UINT16>(y, x) = readDepth.at<UINT16>(y, x + 240);
				}
			}
			cv::imshow("bb", resizeDepth);
			cv::imwrite("resize_depth_image.png", resizeDepth);
		}*/
		key = cv::waitKey(1); //ウィンドウの更新
	}

	return 0;
}