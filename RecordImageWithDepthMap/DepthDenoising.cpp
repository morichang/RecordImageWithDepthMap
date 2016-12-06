//#pragma once

#include <omp.h>

#include "DepthDenoising.h"

DepthDenoising::DepthDenoising(cv::Size depth){
	this->nFrame = 900;
	this->depth = depth;
}

void DepthDenoising::init(){
	frameCount = 0;
	frameFlag = false;
	this->Kd = 1 / nFrame;
	this->nFrameQue.clear();
	this->pop_depth_buffer.clear();
}

bool DepthDenoising::allowDenoise(std::vector<UINT16> &depth_buffer, int nowFrame){
	if (frameFlag){
		return true;
	}

	frameCount = nowFrame;
	storeNFrameBuffer(depth_buffer);
	std::cout << "なーう!ばっふぁーさいず:" << depth_buffer.size() << std::endl;
	//std::cout << "なーう!きゅーさいず:" << nFrameQue.size() << std::endl;
	//std::cout << "なーう!ふれーむ:" << frameCount << std::endl;

	return false;
}


void DepthDenoising::storeNFrameBuffer(std::vector<UINT16> &depth_buffer){
	nFrameQue.push_back(depth_buffer);

	if (frameCount == nFrame){
		std::cout << nFrame << "溜まった! IGZO~!!" << std::endl;
		frameFlag = true;
	}

	return ;
}

void DepthDenoising::simpleMovingAverage(std::vector<UINT16> &depth_buffer){
	
	if (!pop_depth_buffer.empty()){
		for (int i = 0; i < depth.width * depth.height; i++){
			movingAverage[i] -= (long)pop_depth_buffer[i];
			movingAverage[i] += (long)depth_buffer[i];
		}
	}
	else{
		for (auto nFrameBuffer : nFrameQue){
			for (int i = 0; i < depth.width * depth.height; i++){
				movingAverage[i] += (long)nFrameBuffer[i];
			}
		}
	}

	pop_depth_buffer = nFrameQue.front(); //先頭に入ってたバッファを出してやる
	nFrameQue.pop_front(); // 先頭のバッファを殺る
	nFrameQue.push_back(depth_buffer); // 現フレームのバッファを溜める

	return;
}

void DepthDenoising::depthDenoisingFilter(std::vector<UINT16> &filter_depth){
	filter_depth.clear();

	// 移動平均で計算した値をバッファに入れる
	for (int i = 0; i < depth.width * depth.height; i++){
		SMA = (UINT16)(movingAverage[i] * Kd);
		filter_depth[i] = SMA;
	}
	return;
}