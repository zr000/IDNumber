#pragma once
#include <opencv2/imgcodecs.hpp>

using namespace cv;

class CCardImg
{
public:
	CCardImg(string strFullPath);
	~CCardImg();

private:
	// 原始图像
	Mat m_origImg;
	// 文件路径
	string m_strFullPath;


public:
	// 读取身份证图像
	bool ReadCardImg();
	
};

