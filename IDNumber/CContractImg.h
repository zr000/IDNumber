#pragma once

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

class CContractImg
{
public:
	CContractImg();
	~CContractImg();
protected:
	cv::Mat m_imgOrig;
	cv::Mat m_imgBinary;
	cv::Mat m_imgPaint;


	vector<cv::Vec4i> m_horiLine;
	vector<cv::Vec4i> m_verLine;

	//根据矩形范围确定文字内容
	string Img2String(cv::Mat);

public:
	bool FindLines(cv::Mat & img, vector<cv::Vec4i>& hline, vector<cv::Vec4i>& vline);
	cv::Mat GetImgFromHoriLine(vector<cv::Vec4i>& lines, cv::Mat & orig, int l1, int l2);
	
	bool ReadImg(string& imgPath);

	

	
	
	string CustomName();
	string PhoneNum();

	string BusinessContent();

	

	

};

