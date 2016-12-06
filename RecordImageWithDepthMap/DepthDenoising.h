//#pragma once

#include <fstream>
#include <deque>

#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>

#include <Kinect.h>
#include "../../Kinect2Wrapper/Kinect2Wrapper/Kinect2.h"

class DepthDenoising{
private:
	cv::Size depth;

	// �ړ����ςŎZ�o�����l
	UINT16 SMA;
	// nFrame���̈ړ����ς�~�ς���o�b�t�@
	std::vector<unsigned long> movingAverage;
	// nFrame�̐擪����Pop����std::vector<UINT16>�̃o�b�t�@
	std::vector<UINT16> pop_depth_buffer;
	// nFrame����Depth��~�ς���que
	std::deque<std::vector<UINT16>> nFrameQue;
	// nFrame����Depth��~�ς���o�b�t�@
	//std::vector<long> nFrameBuffer;

	// �~�σt���[���̃J�E���g
	int frameCount;
	// �ړ����ς̂��߂̃t���[�����C30fps�Ȃ�1�b����100�t���[�����炢���ǂ��炵��
	int nFrame;
	// �ړ����ς����Z����Ȃ���Z�ŋ��߂邽�߂̌W��
	double Kd;
	// 100Frame��Flag
	bool frameFlag;

public:
	DepthDenoising(cv::Size depth);

	void init();

	bool allowDenoise(std::vector<UINT16> &depth_buffer, int nowFrame);

	void storeNFrameBuffer(std::vector<UINT16> &depth_buffer);

	void simpleMovingAverage(std::vector<UINT16> &depth_buffer);

	void depthDenoisingFilter(std::vector<UINT16> &filter_depth);
};