//#pragma once

#include "KinectPointManager.h"

KinectPointManager::KinectPointManager(cv::Size color, cv::Size depth){
	this->color = color;
	this->depth = depth;
	std::cout << color.width << "," << color.height << std::endl;
	std::cout << depth.width << "," << depth.height << std::endl;
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

// Kinect�Ŏ擾�����f�[�^��Image��DepthImage�ɂ���
void KinectPointManager::convKinectDataToImage(Kinect2Sensor &kinect, cv::Mat &image, cv::Mat &raw_depth, cv::Mat &high_depth){ // Data��Image��DepthMap�ɕۑ�����?
	int height = 0, width = 0;
	std::vector<DepthSpacePoint> depthSpace(color.width * color.height);
	HRESULT hResult = S_OK;
	// Image��Mat���܂������
	if (!image.empty() && !raw_depth.empty()){
		image = cv::Mat::zeros(color, CV_8UC3);
		raw_depth = cv::Mat::zeros(depth, CV_16UC1);
	}
	image = kinect.getFrame();

	std::cout << image.channels() << std::endl;

	auto depth_buffer = kinect.getDepthBuffer();

	// depth_buffer to cv::Mat (Raw Depth Image)
	raw_depth = cv::Mat(cv::Size(depth.width, depth.height), CV_16UC1, &depth_buffer[0]).clone();

	hResult = kinect.getCoordinateMapper()->MapColorFrameToDepthSpace(depth_buffer.size(), &depth_buffer[0], depthSpace.size(), &depthSpace[0]);

	if (FAILED(hResult)) {
		std::cerr << "Error : ICoordinateMapper::MapColorFrameToDepthSpace()" << std::endl;
	}

	// Mapping Depth to Color Resolution
	std::vector<UINT16> buffer(color.width * color.height);

	for (int colorY = 0; colorY < color.height; colorY++){
		for (int colorX = 0; colorX < color.width; colorX++){
			unsigned int colorIndex = colorY * color.width + colorX;
			int depthX = static_cast<int>(depthSpace[colorIndex].X + 0.5f);
			int depthY = static_cast<int>(depthSpace[colorIndex].Y + 0.5f);
			if ((0 <= depthX) && (depthY < depth.width) && (0 <= depthY) && (depthY < depth.height)){
				unsigned int depthIndex = depthY * depth.width + depthX;
				buffer[colorIndex] = depth_buffer[depthIndex];
			}
		}
	}

	// buffer to cv::Mat (Convert to Color Resolution Depth Image)
	high_depth = cv::Mat(color, CV_16UC1, &buffer[0]).clone();

	// Iterator�g����depth�Ƀf�[�^����Ă����ǈӖ��Ȃ�����www
	/*for (auto itr = depth_buffer.begin(); itr != depth_buffer.end(); itr++,width++){
		if (height == 0 || (width / kinect.getDepthSize().width) != 0){
			depth.at<UINT16>(height, width) = *itr;
		}
		else if ((width / kinect.getDepthSize().width) == 0){
			height += 1;
			width = 0;
			depth.at<UINT16>(height, width) = *itr;
		}
	}*/
}

// DepthMap�����𑜓x�ɂ���(���n�̎擾����Ȃ������������ɂȂ�΁c)
void KinectPointManager::depthmapRefinement(cv::Mat image, cv::Mat srcDepth){

}