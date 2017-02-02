//#pragma once

#include <omp.h>

#include "KinectPointManager.h"
#include "DepthDenoising.h"

KinectPointManager::KinectPointManager(cv::Size color, cv::Size depth)
{
	hResult = S_OK;
	colorW = color.width;
	colorH = color.height;
	depthW = depth.width;
	depthH = depth.height;
	this->color = color;
	this->depth = depth;
	depthSpace.resize(colorW * colorH);
	buffer.resize(colorW * colorH);
	//this->initFlag = true;
	//this->frameCount = 0;
}

// Kinectで取得したデータをImageとDepthMapにする
void KinectPointManager::convKinectDataToImage(Kinect2Sensor &kinect, cv::Mat &color_res_depth)
{
	// ImageとDepthのMatを初期化
	if (!color_res_depth.empty()) {
		color_res_depth = cv::Mat::zeros(color, CV_16UC1);
	}

	// depth_bufferに現フレームのバッファを入れる
	std::vector<UINT16> depth_buffer = kinect.getDepthBuffer();

	//hResult = kinect.getCoordinateMapper()->MapColorFrameToDepthSpace(depth_buffer.size(), &depth_buffer[0], depthSpace.size(), &depthSpace[0]);
	//if (FAILED(hResult)) {
	//	std::cerr << "Error : ICoordinateMapper::MapColorFrameToDepthSpace()" << std::endl;
	//}
	
	// Mapping Depth to Color Resolution
	// OpenMPでループの並列化 (速度が早くなる?)
	#pragma omp parallel for
	for (int colorY = 0; colorY < colorH; ++colorY) {
		#pragma omp parallel for
		for (int colorX = 0; colorX < colorW; ++colorX) {
			unsigned int colorIndex = colorY * colorW + colorX;
			// 0.5fで四捨五入
			//int depthX = static_cast<int>(depthSpace[colorIndex].X + 0.5f);
			//int depthY = static_cast<int>(depthSpace[colorIndex].Y + 0.5f);
			//if ((0 <= depthX) && (depthX < depthW) && (0 <= depthY) && (depthY < depthH)) {
			//	unsigned int depthIndex = depthY * depthW + depthX;
			//	buffer[colorIndex] = depth_buffer[depthIndex];
			//}
			buffer[colorIndex] = kinect.getDepthValueFromColor(colorIndex);
		}
	}

	// buffer to cv::Mat (Convert to Color Resolution Depth Image)
	color_res_depth = cv::Mat(color, CV_16UC1, &buffer[0]).clone();
}

// Color ImageをDepthの解像度に落としこむ
void KinectPointManager::convImageResolutionToDepthSize(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &depth_res_image)
{
	std::vector<ColorSpacePoint> colorSpace(depth.width * depth.height);
	// ImageとRaw DepthのMatを初期化
	if (!image.empty() && !depth_res_image.empty()) {
		image = cv::Mat::zeros(depth, CV_8UC4);
		depth_res_image = cv::Mat::zeros(depth, CV_8UC3);
	}

	colorBytesPerPixel = kinect.getColorBytesPerPixels();

	auto color_buffer = kinect.getColorBuffer();
	auto depth_buffer = kinect.getDepthBuffer();

	hResult = kinect.getCoordinateMapper()->MapDepthFrameToColorSpace(depth_buffer.size(), &depth_buffer[0], colorSpace.size(), &colorSpace[0]);

	if (FAILED(hResult)) {
		std::cerr << "Error : ICoordinateMapper::MapDepthFrameToColorSpace()" << std::endl;
	}
	// Mapping Depth to Color Resolution
	std::vector<BYTE> buffer(depth.width * depth.height * colorBytesPerPixel);

	// OpenMPでループの並列化 (速度が早くなる?)
	#pragma omp parallel for
	for (int depthY = 0; depthY < depth.height; ++depthY) {
		#pragma omp parallel for
		for (int depthX = 0; depthX < depth.width; ++depthX) {
			unsigned int depthIndex = depthY * depth.width + depthX;
			// 0.5fで四捨五入
			int colorX = static_cast<int>(colorSpace[depthIndex].X + 0.5f);
			int colorY = static_cast<int>(colorSpace[depthIndex].Y + 0.5f);
			if ((0 <= colorX) && (colorX < color.width) && (0 <= colorY) && (colorY < color.height)) {
				unsigned int colorIndex = colorY * color.width + colorX;
				buffer[depthIndex * colorBytesPerPixel + 0] = color_buffer[colorIndex * colorBytesPerPixel + 0];
				buffer[depthIndex * colorBytesPerPixel + 1] = color_buffer[colorIndex * colorBytesPerPixel + 1];
				buffer[depthIndex * colorBytesPerPixel + 2] = color_buffer[colorIndex * colorBytesPerPixel + 2];
				buffer[depthIndex * colorBytesPerPixel + 3] = color_buffer[colorIndex * colorBytesPerPixel + 3];
			}
		}
	}

	image = cv::Mat(depth, CV_8UC4, &buffer[0]).clone();
	cv::cvtColor(image, depth_res_image, CV_BGRA2BGR);
}