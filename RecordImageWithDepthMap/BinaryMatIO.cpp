//#pragma once

#include "BinaryMatIO.h"

BinaryMatIO::BinaryMatIO()
{
	char szComputerName[256] = { '\0' };
	DWORD dwSize = sizeof(szComputerName) / sizeof(szComputerName[0]);
	GetComputerName(szComputerName, &dwSize);
	computerName = std::string(szComputerName);
}

void BinaryMatIO::recordInitialize(std::string filename, bool flag)
{
	this->filename = filename;
	imageWidth = 1920;
	imageHeight = 1080;
	colorType = CV_8UC3;
	depthType = CV_16UC1;

	if (flag) {
		ofs.open(filename, std::ios::out | std::ios::binary);
		if (ofs.fail()) {
			std::cout << "保存ファイルオープン失敗" << std::endl;
			return;
		}
		// HeaderとしてColorのrowsとcolsとDataType，DepthのrowsとcolsとDataTypeの書き込みをする
		// ColorのHeader書き込み
		ofs.write((const char*)(&imageHeight), sizeof(int));
		ofs.write((const char*)(&imageWidth), sizeof(int));
		ofs.write((const char*)(&colorType), sizeof(int));
		// DepthのHeader書き込み
		//ofs.write((const char*)(&imageHeight), sizeof(int));
		//ofs.write((const char*)(&imageWidth), sizeof(int));
		ofs.write((const char*)(&depthType), sizeof(int));
	}
	else {
		ifs.open(filename, std::ios::in | std::ios::binary);
		if (ifs.fail()) {
			std::cout << "読み込みファイルオープン失敗" << std::endl;
			return;
		}
	}

	// 画像のサイズ
	resolution = imageWidth*imageHeight;

	// 書き込む時に使うelemSizeここで定義する
	colorElemSize = 3;  // 3byte
	depthElemSize = 2;  // 2byte
	
	nowBuffer = 0;
	lastUpdate = boost::posix_time::microsec_clock::local_time();
	targetFPS = 30;
	// 最大のバッファサイズ64MB
	maxBufferSize = 1024 * 1024 * 128;
}

void BinaryMatIO::setData(cv::Mat &colorFrame, cv::Mat &depthFrame)
{
	if (!ofs.is_open()) {
		std::cout << "おかしいぞコラ!" << std::endl;
		return;
	}

	// 今の時間を得る
	boost::posix_time::ptime nowTime = boost::posix_time::microsec_clock::local_time();

	// 保存すべき時刻かどうかチェック
	double interval = 1000.0 / targetFPS;
	if ((nowTime - lastUpdate).total_milliseconds() < interval) {
		return;
	}
	// 最終更新時刻を更新する
	lastUpdate = nowTime;

	// リストに現フレームを入れる
	colorFrameList.push_back(colorFrame);
	depthFrameList.push_back(depthFrame);
	timeList.push_back(nowTime);

	// いつ書き込むか?
	nowBuffer += (resolution*sizeof(cv::Vec3b)) + (resolution*sizeof(UINT16));
	if (nowBuffer >= maxBufferSize) {
		flush();
		nowBuffer = 0;
	}
}

void BinaryMatIO::recordClose()
{
	std::cout << "閉じます" << std::endl;
	flush();
	if (ofs.is_open()) {
		ofs.close();
	}
	else if (ifs.is_open()) {
		ifs.close();
	}
}

void BinaryMatIO::flush()
{
	if (!ofs.is_open())
	{
		return;
	}

	std::vector<boost::posix_time::ptime>::iterator itrTime = timeList.begin();
	std::vector<cv::Mat>::iterator itrColor = colorFrameList.begin();
	std::vector<cv::Mat>::iterator itrDepth = depthFrameList.begin();

	// Header的な感じでデータ以外のことをInitializeで書き込んで，データだけを書き込むことで高速化
	while (itrTime != timeList.end())	{
		// 時間を書き込む
		ofs.write((const char*)&(*itrTime), sizeof(boost::posix_time::ptime));

		// カラーフレームを書き込む
		ofs.write((const char*)((*itrColor).data), colorElemSize*resolution);

		// デプスフレームを書き込む
		ofs.write((const char*)((*itrDepth).data), depthElemSize*resolution);

		++itrDepth;
		++itrColor;
		++itrTime;
	}

	depthFrameList.clear();
	colorFrameList.clear();
	timeList.clear();
}

void BinaryMatIO::readData()
{
	if (!ifs.is_open()) {
		std::cout << "おかしいぞ~!" << std::endl;
		return;
	}

	int rows, cols, colorType;
	ifs.read((char *)(&rows), sizeof(int));
	if (rows == 0)	{
		return;
	}
	ifs.read((char *)(&cols), sizeof(int));
	ifs.read((char *)(&colorType), sizeof(int));

	int depthType;
	ifs.read((char *)(&depthType), sizeof(int));

	while (!ifs.eof()) {
		int index = 0;
		std::string split;
		cv::Mat color, depth;
		auto facet = new boost::posix_time::time_facet("%Y-%m-%d_%H-%M-%S%F");  // 2017-01-09_03:24:00.000000
		oss.imbue(std::locale(oss.getloc(), facet));

		boost::posix_time::ptime time;
		ifs.read((char *)(&time), sizeof(boost::posix_time::ptime));

		// facetで直された時間の秒以下の表現を "." から "_" に変換する(検索しやすくする)
		oss << time;
		split = oss.str();
		while ((index = split.find(".")) != -1) {
			split.replace(index, 1, "_");
		}

		//std::cout << split << std::endl;

		// Color書き出し
		color.release();
		color.create(rows, cols, colorType);
		ifs.read((char *)(color.data), color.elemSize()*color.total());

		// Depth書き出し
		depth.release();
		depth.create(rows, cols, depthType);
		ifs.read((char *)(depth.data), depth.elemSize()*depth.total());

		if (!(split == "not-a-date-time")) {
			cv::imshow("Color", color);
			//cv::imwrite("./test/color/" + split + "-color.png", color);
			//cv::imwrite("./test/depth/" + split + "-depth.png", depth);
			Sleep(33);
		}

		// ストリングストリームのバッファの状態をクリアする
		oss.str("");
		oss.clear(std::stringstream::goodbit);
	}
	ifs.close();
	std::cout << "解凍終了" << std::endl;
}