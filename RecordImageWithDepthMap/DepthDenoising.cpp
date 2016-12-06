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
	std::cout << "�ȁ[��!�΂��ӂ��[������:" << depth_buffer.size() << std::endl;
	//std::cout << "�ȁ[��!����[������:" << nFrameQue.size() << std::endl;
	//std::cout << "�ȁ[��!�ӂ�[��:" << frameCount << std::endl;

	return false;
}


void DepthDenoising::storeNFrameBuffer(std::vector<UINT16> &depth_buffer){
	nFrameQue.push_back(depth_buffer);

	if (frameCount == nFrame){
		std::cout << nFrame << "���܂���! IGZO~!!" << std::endl;
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

	pop_depth_buffer = nFrameQue.front(); //�擪�ɓ����Ă��o�b�t�@���o���Ă��
	nFrameQue.pop_front(); // �擪�̃o�b�t�@���E��
	nFrameQue.push_back(depth_buffer); // ���t���[���̃o�b�t�@�𗭂߂�

	return;
}

void DepthDenoising::depthDenoisingFilter(std::vector<UINT16> &filter_depth){
	filter_depth.clear();

	// �ړ����ςŌv�Z�����l���o�b�t�@�ɓ����
	for (int i = 0; i < depth.width * depth.height; i++){
		SMA = (UINT16)(movingAverage[i] * Kd);
		filter_depth[i] = SMA;
	}
	return;
}