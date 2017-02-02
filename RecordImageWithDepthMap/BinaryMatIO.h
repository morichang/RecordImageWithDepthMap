//#pragma once

#include <fstream>

#include <list>

#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>

class BinaryMatIO{
private:
	std::string filename;  // �t�@�C���̖��O
	std::ostringstream oss;
	std::ofstream ofs;
	std::ifstream ifs;

	std::vector<boost::posix_time::ptime> timeList;  // �L�^��������
	std::vector<cv::Mat> colorFrameList;  // �J���[�t���[���̏W�܂�
	std::vector<cv::Mat> depthFrameList;  // �f�v�X�t���[���̏W�܂�

	size_t maxBufferSize;  // �ő�o�b�t�@��
	size_t nowBuffer;	  // ���̃o�b�t�@

	boost::posix_time::ptime lastUpdate;  // �ŏI�X�V����
	int resolution;  //�𑜓x
	int imageWidth;  //Image��width
	int imageHeight;  //Image��height
	int colorType;  // Color��DataType
	int depthType;  // Depth��DataType
	int colorElemSize;  // Color��1�v�f�̃T�C�Y[byte]
	int depthElemSize;  // Depth��1�v�f�̃T�C�Y[byte]
	double targetFPS;  // fps

	void flush();

public:
	std::string computerName; // �R���s���[�^��

	BinaryMatIO();
	void recordInitialize(std::string filename, bool flag);
	void setData(cv::Mat &colorFrame, cv::Mat &depthFrame);
	void recordClose();

	void readData();
};