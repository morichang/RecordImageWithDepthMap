//#pragma once

#include <fstream>

#include <list>

#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>

class BinaryMatIO{
private:
	std::string filename;  // ファイルの名前
	std::ostringstream oss;
	std::ofstream ofs;
	std::ifstream ifs;

	std::vector<boost::posix_time::ptime> timeList;  // 記録した時間
	std::vector<cv::Mat> colorFrameList;  // カラーフレームの集まり
	std::vector<cv::Mat> depthFrameList;  // デプスフレームの集まり

	size_t maxBufferSize;  // 最大バッファ数
	size_t nowBuffer;	  // 今のバッファ

	boost::posix_time::ptime lastUpdate;  // 最終更新時刻
	int resolution;  //解像度
	int imageWidth;  //Imageのwidth
	int imageHeight;  //Imageのheight
	int colorType;  // ColorのDataType
	int depthType;  // DepthのDataType
	int colorElemSize;  // Colorの1要素のサイズ[byte]
	int depthElemSize;  // Depthの1要素のサイズ[byte]
	double targetFPS;  // fps

	void flush();

public:
	std::string computerName; // コンピュータ名

	BinaryMatIO();
	void recordInitialize(std::string filename, bool flag);
	void setData(cv::Mat &colorFrame, cv::Mat &depthFrame);
	void recordClose();

	void readData();
};