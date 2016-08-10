#pragma once

#include "KinectPointManager.h"

void KinectPointManager::recordInit(std::string filename){
	this->filename = filename;

	ofs.open(filename, std::ios::out || std::ios::trunc);
	if (ofs.fail()){
		std::cout << "File Open Fail" << std::endl;
		return;
	}

	nowBuffer = 0;
	lastUpdate = boost::posix_time::microsec_clock::local_time();
	targetFPS = 30;
	maxBufferSize = 1024 * 1024 * 128; // 64MBCª‹’‚È‚µ?
}

void KinectPointManager::recordClose(){
	std::cout << "File Close" << std::endl;
	flush();
	ofs.close();
}

void KinectPointManager::setData(){

}

void KinectPointManager::convKinectDataToImage(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &depth){ // Data‚ðImage‚ÆDepthMap‚É•Û‘¶‚·‚é?
	cv::Mat frameData = kinect.getFrame();
	unsigned char *colorImagePointer = frameData.data;

	for (int y = 0; y < kinect.getDepthSize().height; y++){
		for (int x = 0; x < kinect.getDepthSize().width; x++){
			cv::Point colorPoint = kinect.getMapper()->getColorUVFromCameraPoint;
			if (0 <= colorPoint.x && colorPoint.x < kinect.getColorSize().width && 0 <= colorPoint.y && colorPoint.y < kinect.getColorSize().height){
				// image‚ÉRGBî•ñ‚ð“ü‚ê‚Ä‚¢‚­
				unsigned int index = (colorPoint.y * kinect.getColorSize().width + colorPoint.x) * 3;
				// ƒJƒ‰[‚ð“ü‚ê‚Ä‚¢‚­
				image.data[index + 0] = colorImagePointer[index + 0];
				image.data[index + 1] = colorImagePointer[index + 1];
				image.data[index + 2] = colorImagePointer[index + 2];
			}
			cv::imshow("Color Image", image);
		}
	}

}

void KinectPointManager::depthmapRefinement(cv::Mat image, cv::Mat srcDepth){

}