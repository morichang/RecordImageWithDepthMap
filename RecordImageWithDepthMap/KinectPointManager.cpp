//#pragma once

#include <omp.h>

#include "KinectPointManager.h"
#include "DepthDenoising.h"

KinectPointManager::KinectPointManager(cv::Size color, cv::Size depth){
	this->color = color;
	this->depth = depth;
	this->initFlag = true;
	this->frameCount = 0;
}

// �L�^����t�@�C�����J��(����)
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
	maxBufferSize = 1024 * 1024 * 128; // 64MB�C�����Ȃ�?
}

// �L�^�����t�@�C�������(�ۑ�)
void KinectPointManager::recordClose(){
	std::cout << "File Close" << std::endl;
	flush();
	ofs.close();
}

// �t�@�C���ɏ������ރf�[�^�̏���
void KinectPointManager::setData(){
	// �t�@�C�����J����Ă��邩�m�F
	if (!ofs.is_open())
	{
		std::cout << "�J���ĂȂ������?" << std::endl;
	}
	// ���ݎ����̎擾
	auto nowTime = boost::posix_time::microsec_clock::local_time();
	//�ۑ����ׂ��������ǂ����`�F�b�N
	double interval = 1000.0 / targetFPS;
	if ((nowTime - lastUpdate).total_milliseconds() < interval) {
		return;
	}
	// �X�V�����Ɍ��ݎ������Z�b�g
	lastUpdate = nowTime;

	// �����ŏ������ރf�[�^��p�ӂ���

}

// �f�[�^���t�@�C���ɏ������� (BinaryCvMat���Q�l��)
void KinectPointManager::flush(){

}

// Kinect�Ŏ擾�����f�[�^��Image��DepthMap�ɂ���
void KinectPointManager::convKinectDataToImage(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &raw_depth, cv::Mat &high_depth, std::vector<UINT16> &depthBuffer){ // Data��Image��DepthMap�ɕۑ�����?
	DepthDenoising denoise(depth);
	HRESULT hResult = S_OK;
	std::vector<DepthSpacePoint> depthSpace(color.width * color.height);
	if (initFlag){
		denoise.init();
		std::cout << "init��RYO!" << std::endl;
		initFlag = false;
	}
	// Image��Raw Depth��Mat��������
	if (!image.empty() && !raw_depth.empty() && !high_depth.empty()){
		image = cv::Mat::zeros(color, CV_8UC3);
		raw_depth = cv::Mat::zeros(depth, CV_16UC1);
		high_depth = cv::Mat::zeros(color, CV_16UC1);
	}
	// Image�Ɍ��t���[���̉摜������
	image = kinect.getFrame();

	// depth_buffer�Ɍ��t���[���̃o�b�t�@������
	auto depth_buffer = kinect.getDepthBuffer();

	// depth_buffer to cv::Mat (Raw Depth Image)
	raw_depth = cv::Mat(cv::Size(depth.width, depth.height), CV_16UC1, &depth_buffer[0]).clone();

	hResult = kinect.getCoordinateMapper()->MapColorFrameToDepthSpace(depth_buffer.size(), &depth_buffer[0], depthSpace.size(), &depthSpace[0]);

	if (FAILED(hResult)) {
		std::cerr << "Error : ICoordinateMapper::MapColorFrameToDepthSpace()" << std::endl;
	}

	// Mapping Depth to Color Resolution
	std::vector<UINT16> buffer(color.width * color.height);

	// OpenMP�Ń��[�v�̕��� (���x�������Ȃ�?)
#pragma omp parallel for
	for (int colorY = 0; colorY < color.height; colorY++){
		for (int colorX = 0; colorX < color.width; colorX++){
			unsigned int colorIndex = colorY * color.width + colorX;
			// 0.5f�Ŏl�̌ܓ�
			int depthX = static_cast<int>(depthSpace[colorIndex].X + 0.5f);
			int depthY = static_cast<int>(depthSpace[colorIndex].Y + 0.5f);
			if ((0 <= depthX) && (depthX < depth.width) && (0 <= depthY) && (depthY < depth.height)){
				unsigned int depthIndex = depthY * depth.width + depthX;
				buffer[colorIndex] = depth_buffer[depthIndex];
			}
		}
	}

	// buffer to cv::Mat (Convert to Color Resolution Depth Image)
	high_depth = cv::Mat(color, CV_16UC1, &buffer[0]).clone();
	depthBuffer = buffer;
	frameCount++;
}

// Color Image��Depth�̉𑜓x�ɗ��Ƃ�����
void KinectPointManager::convImageResolutionToDepthSize(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &depth_res_image){
	HRESULT hResult = S_OK;
	std::vector<ColorSpacePoint> colorSpace(depth.width * depth.height);
	// Image��Raw Depth��Mat��������
	if (!image.empty() && !depth_res_image.empty()){
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

	// OpenMP�Ń��[�v�̕��� (���x�������Ȃ�?)
#pragma omp parallel for
	for (int depthY = 0; depthY < depth.height; depthY++){
		for (int depthX = 0; depthX < depth.width; depthX++){
			unsigned int depthIndex = depthY * depth.width + depthX;
			// 0.5f�Ŏl�̌ܓ�
			int colorX = static_cast<int>(colorSpace[depthIndex].X + 0.5f);
			int colorY = static_cast<int>(colorSpace[depthIndex].Y + 0.5f);
			if ((0 <= colorX) && (colorX < color.width) && (0 <= colorY) && (colorY < color.height)){
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

// DepthMap�����𑜓x�ɂ���(���n�̎擾����Ȃ������������ɂȂ�΁c)
void KinectPointManager::depthmapRefinement(cv::Mat image, cv::Mat srcDepth){

}