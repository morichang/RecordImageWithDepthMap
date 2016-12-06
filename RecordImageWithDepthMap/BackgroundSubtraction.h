/*
�[�x�l���g�����w�i����
*/


class BackgroundSubtraction{
private:

	cv::Mat backgroundMap;		//�w�i�����p�̃x�[�X
	cv::Mat foreground;			//�w�i�����ňړ������Ƃ���

public:
	/*
	depthSize: �[�x�Z���T�̉𑜓x
	*/
	BackgroundSubtraction(cv::Size depthSize);
	

	/*
	�w�i�����̍X�V
	*/
	void update(std::vector<UINT16> depth);

	void updateImageAndDepth(cv::Mat &srcImage, cv::Mat &nowImage, cv::Mat &splitImage, cv::Mat &srcDepth, cv::Mat &nowDepth, cv::Mat &splitDepth);

	cv::Mat getForeground();

	void foregroundColor(cv::Mat& image, cv::Mat& result, cv::Size depthSize);
};