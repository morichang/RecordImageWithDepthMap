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

	// 移動平均で算出した値
	UINT16 SMA;
	// nFrame分の移動平均を蓄積するバッファ
	std::vector<unsigned long> movingAverage;
	// nFrameの先頭からPopしたstd::vector<UINT16>のバッファ
	std::vector<UINT16> pop_depth_buffer;
	// nFrame分のDepthを蓄積するque
	std::deque<std::vector<UINT16>> nFrameQue;
	// nFrame分のDepthを蓄積するバッファ
	//std::vector<long> nFrameBuffer;

	// 蓄積フレームのカウント
	int frameCount;
	// 移動平均のためのフレーム数，30fpsなら1秒分の100フレームぐらいが良いらしい
	int nFrame;
	// 移動平均を除算じゃなく乗算で求めるための係数
	double Kd;
	// 100FrameのFlag
	bool frameFlag;

public:
	DepthDenoising(cv::Size depth);

	void init();

	bool allowDenoise(std::vector<UINT16> &depth_buffer, int nowFrame);

	void storeNFrameBuffer(std::vector<UINT16> &depth_buffer);

	void simpleMovingAverage(std::vector<UINT16> &depth_buffer);

	void depthDenoisingFilter(std::vector<UINT16> &filter_depth);
};