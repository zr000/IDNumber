#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

class CCardImg
{
public:
	CCardImg(string strFullPath);
	~CCardImg();

private:
	
	// 原始图像
	
	// 文件路径
	string m_strFullPath;

	CSize m_imgSize;

public:
	// 读取身份证图像
	bool ReadCardImg();
	Mat m_origImg;


#ifdef _WIN32
	void DrawImg(cv::Mat& mat, CDC* dc, CRect rect);
	void CCardImg::MatToCImage(Mat &mat, CImage &cImage);
#endif // _WIN32
};

