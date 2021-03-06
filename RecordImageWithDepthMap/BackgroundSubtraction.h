/*
深度値を使った背景差分
*/


class BackgroundSubtraction{
private:

	cv::Mat backgroundMap;		//背景差分用のベース
	cv::Mat foreground;			//背景差分で移動したところ

public:
	/*
	depthSize: 深度センサの解像度
	*/
	BackgroundSubtraction(cv::Size depthSize);
	

	/*
	背景差分の更新
	*/
	void update(std::vector<UINT16> depth);

	void updateImageAndDepth(cv::Mat &srcImage, cv::Mat &nowImage, cv::Mat &splitImage, cv::Mat &srcDepth, cv::Mat &nowDepth, cv::Mat &splitDepth);

	cv::Mat getForeground();

	void foregroundColor(cv::Mat& image, cv::Mat& result, cv::Size depthSize);
};