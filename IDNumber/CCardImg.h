#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

class CCardImg
{
public:
	CCardImg(string strFullPath);
	~CCardImg();

private:
	
	
	
	// 文件路径
	string m_strFullPath;

	CSize m_imgSize;

public:
	// 读取身份证图像
	bool ReadCardImg();

	// 原始图像
	Mat m_origImg;

	// 二值化图像
	Mat m_binaryImg;

	// 定位
	bool Position();

#ifdef _WIN32
	void DrawImg(cv::Mat& mat, CDC* dc, CRect rect);
	void CCardImg::MatToCImage(Mat &mat, CImage &cImage);
#endif // _WIN32
};

