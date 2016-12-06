/*
[“x’l‚ğg‚Á‚½”wŒi·•ª
*/


class BackgroundSubtraction{
private:

	cv::Mat backgroundMap;		//”wŒi·•ª—p‚Ìƒx[ƒX
	cv::Mat foreground;			//”wŒi·•ª‚ÅˆÚ“®‚µ‚½‚Æ‚±‚ë

public:
	/*
	depthSize: [“xƒZƒ“ƒT‚Ì‰ğ‘œ“x
	*/
	BackgroundSubtraction(cv::Size depthSize);
	

	/*
	”wŒi·•ª‚ÌXV
	*/
	void update(std::vector<UINT16> depth);

	void updateImageAndDepth(cv::Mat &srcImage, cv::Mat &nowImage, cv::Mat &splitImage, cv::Mat &srcDepth, cv::Mat &nowDepth, cv::Mat &splitDepth);

	cv::Mat getForeground();

	void foregroundColor(cv::Mat& image, cv::Mat& result, cv::Size depthSize);
};