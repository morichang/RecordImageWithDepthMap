//#pragma once

#include <fstream>
#include <list>

#include <boost/date_time.hpp>

#include <Kinect.h>
#include "../../Kinect2Wrapper/Kinect2Wrapper/Kinect2.h"
#include "WaveFile.h"

class KinectAudioManager{
private:
	IAudioBeamFrameReader* audioBeamFrameReader;
	WaveFile audioFile;

	HRESULT hResult;

	UINT subFrameLengthInBytes;

	std::list<boost::posix_time::ptime> timeList;
	std::vector<BYTE> audioBuffer;
	
public:
	void audioInitialize(Kinect2Sensor &kinect);
	void audioUpdate();
	void audioClose();
};