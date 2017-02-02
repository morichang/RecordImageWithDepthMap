//#pragma once

#include "KinectAudioManager.h"

void KinectAudioManager::audioInitialize(Kinect2Sensor &kinect){
	audioBeamFrameReader = kinect.getAaudioBeamFrameReader();
	subFrameLengthInBytes = 0;
	subFrameLengthInBytes= kinect.getSubFrameLengthInBytes();
	audioBuffer.resize(subFrameLengthInBytes);

	audioFile.Open("./test/morichang.wav");
}

void KinectAudioManager::audioUpdate(){
	ComPtr<IAudioBeamFrameList> audioBeamFrameList;
	hResult = S_OK;

	hResult = audioBeamFrameReader->AcquireLatestBeamFrames(&audioBeamFrameList);
	if (hResult != S_OK){
		return;
	}

	// ビームフレームを取得する
	UINT beamCount = 0;
	hResult = audioBeamFrameList->get_BeamCount(&beamCount);
	if (FAILED(hResult)) {
		std::cerr << "Error : IAudioBeamFrameList::get_BeamCount()" << std::endl;
	}

	
	UINT subFrameCount;

	//std::cout << "BeamCount: " << beamCount << std::endl;
	for (int i = 0; i < beamCount; ++i) {
		ComPtr<IAudioBeamFrame> audioBeamFrame;
		hResult = audioBeamFrameList->OpenAudioBeamFrame(i, &audioBeamFrame);
		if (FAILED(hResult)) {
			std::cerr << "Error : IAudioBeamFrameList::OpenAudioBeamFrame()" << std::endl;
		}

		// サブフレームを取得する
		subFrameCount = 0;
		hResult = audioBeamFrame->get_SubFrameCount(&subFrameCount);
		//std::cout << "subFrameCount: " << subFrameCount << std::endl;
		if (FAILED(hResult)) {
			std::cerr << "Error : IAudioBeamFrame::get_SubFrameCount()" << std::endl;
		}

		for (int j = 0; j < subFrameCount; ++j) {
			ComPtr<IAudioBeamSubFrame> audioBeamSubFrame;
			hResult = audioBeamFrame->GetSubFrame(j, &audioBeamSubFrame);
			if (FAILED(hResult)) {
				std::cerr << "Error : IAudioBeamFrame::GetSubFrame()" << std::endl;
			}

			if (audioFile.audioFile.is_open()) {
				audioBeamSubFrame->CopyFrameDataToArray((UINT)subFrameLengthInBytes, &audioBuffer[0]);
				audioFile.Write(&audioBuffer[0], (UINT)subFrameLengthInBytes);
			}
		}
	}
}

void KinectAudioManager::audioClose()
{
	std::cout << "録音終了します!" << std::endl;
	audioFile.Close();
}