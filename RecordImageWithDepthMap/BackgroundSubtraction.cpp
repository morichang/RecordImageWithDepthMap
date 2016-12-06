#include "BackgroundSubtraction.h"


BackgroundSubtraction::BackgroundSubtraction(cv::Size depthSize){
	backgroundMap = cv::Mat::zeros(depthSize, CV_16UC1);  // ゼロ埋め
}

void BackgroundSubtraction::update(std::vector<UINT16> depth){
	foreground = cv::Mat::zeros(backgroundMap.size(), CV_16UC1);  // 初期化

	//DEFAULT 64.0
	const static double THRESHOLD = 64.0;
	//背景差分をします
	for (int i = 0; i < depth.size(); i++){
		if (depth[i] > 0 && abs(backgroundMap.at<UINT16>(i) -depth[i]) > THRESHOLD){
			foreground.at<UINT16>(i) = 65535;				//白くする
		}
	}

	//背景マップを更新する
	const static double ALPHA = 0.01;			//何割くらい現在の深度値を採用するか
	for (int i = 0; i < backgroundMap.size().area(); i++){
		/*
		背景マップより，遠い点が出てきたら，そこは速攻背景にする
		処理速度(fps)より早く後ろに下がると，背景になる恐れあり．
		将来的には，Thresholdの値をdtによって変化させた方がいい
		*/
		if (backgroundMap.at<UINT16>(i) -depth[i] < -THRESHOLD){
			backgroundMap.at<UINT16>(i) = depth[i];
		}
		else{
			if (foreground.at<UINT16>(i) == 65535 && depth[i] > 0){
				backgroundMap.at<UINT16>(i) =
					backgroundMap.at<UINT16>(i) * (1 - ALPHA) +
					depth[i] * ALPHA;
			}
		}
	}

	//前景画像のノイズ除去をする
	//cv::morphologyEx(foreground, foreground, CV_MOP_OPEN, cv::Mat(4, 4, CV_16UC1, cv::Scalar(1)), cv::Point(-1, -1));
	//白領域を膨張させる
	cv::dilate(foreground, foreground, cv::Mat());
	//白領域を収縮させる
	cv::erode(foreground, foreground, cv::Mat());
	//std::cout << foreground.type() << std::endl;
	cv::imshow("foreground", foreground);
}

void BackgroundSubtraction::updateImageAndDepth(cv::Mat &srcImage, cv::Mat &nowImage, cv::Mat &splitImage, cv::Mat &srcDepth, cv::Mat &nowDepth, cv::Mat &splitDepth){
	splitImage = cv::Mat::zeros(1080, 1920, CV_8UC3);
	splitDepth = cv::Mat::zeros(1080, 1920, CV_16UC1);
	const static double THRESHOLD = 30.0;
	for (int y = 0; y < srcImage.rows; y++){
		for (int x = 0; x < srcImage.cols; x++){
			if ((abs(srcImage.at<cv::Vec3b>(y, x)[0] - nowImage.at<cv::Vec3b>(y, x)[0]) < THRESHOLD) && (abs(srcImage.at<cv::Vec3b>(y, x)[1] - nowImage.at<cv::Vec3b>(y, x)[1]) < THRESHOLD) && (abs(srcImage.at<cv::Vec3b>(y, x)[2] - nowImage.at<cv::Vec3b>(y, x)[2]) < THRESHOLD)){
				splitImage.at<cv::Vec3b>(y, x)[0] = 0;
				splitImage.at<cv::Vec3b>(y, x)[1] = 0;
				splitImage.at<cv::Vec3b>(y, x)[2] = 0;
			}
			else{
				splitImage.at<cv::Vec3b>(y, x)[0] = nowImage.at<cv::Vec3b>(y, x)[0];
				splitImage.at<cv::Vec3b>(y, x)[1] = nowImage.at<cv::Vec3b>(y, x)[1];
				splitImage.at<cv::Vec3b>(y, x)[2] = nowImage.at<cv::Vec3b>(y, x)[2];
			}
		}
	}
	for (int y = 0; y < srcDepth.rows; y++){
		for (int x = 0; x < srcDepth.cols; x++){
			if (abs(srcDepth.at<UINT16>(y, x) - nowDepth.at<UINT16>(y, x)) < THRESHOLD){
				splitDepth.at<UINT16>(y, x) = 0;
			}
			else{
				splitDepth.at<UINT16>(y, x) = nowDepth.at<UINT16>(y, x);
			}
		}
	}
}

cv::Mat BackgroundSubtraction::getForeground(){
	return foreground;
}