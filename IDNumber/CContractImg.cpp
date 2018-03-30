#include "stdafx.h"
#include "CContractImg.h"
#include <math.h>
#include <algorithm>

#define PI 3.1415926
#define SPACE_LEN 30

CContractImg::CContractImg()
{
}


CContractImg::~CContractImg()
{
}

//直线位置排序，上，左小于下，右
static bool linePosComp(cv::Vec4i l1, cv::Vec4i l2)
{
	//比较相等的值就可以
	if (l1[0] == l1[2])
		return l1[0] < l2[0];
	if (l1[1] == l1[3])
		return l1[1] < l2[1];

	return false;
}

bool CContractImg::FindLines(cv::Mat& img, vector<cv::Vec4i>& hline, vector<cv::Vec4i>& vline)
{

	//统计概率霍夫变换找长度大于图片宽/2的直线
	vector<cv::Vec4i> lines;
	HoughLinesP(img, lines, 1, CV_PI / 180, 50, img.cols / 2, 10);
	

	hline.clear();
	vline.clear();

	for (size_t i = 0; i < lines.size(); i++)
	{
		cv::Vec4i l = lines[i];
		if (l[0] < img.cols / 18 || l[0] > 17 * img.cols / 18) //去掉最外层直线
			continue;
		cv::Point p1 = cv::Point(l[0], l[1]);
		cv::Point p2 = cv::Point(l[2], l[3]);

		//float angle = atan2(p1.y - p2.y, p1.x - p2.x)*180/PI;
		if (p1.x == p2.x)
			vline.push_back(cv::Vec4i(p1.x, p1.y, p2.x, p2.y));
		else
			hline.push_back(cv::Vec4i(p1.x, p1.y, p2.x, p2.y));

		//line(m_imgPaint, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 2, CV_AA);
	}

	//上下，左右排序
	sort(hline.begin(), hline.end(), linePosComp);
	sort(vline.begin(), vline.end(), linePosComp);

	//去掉不需要的
	for (int i = 0; i < hline.size() - 1; i++) {
		if (hline[i + 1][1] - hline[i][1] < SPACE_LEN) {
			hline.erase(hline.begin() + i + 1);
			i++;
		}
	}
	for (int i = 0; i < vline.size() - 1; i++) {
		if (vline[i + 1][0] - vline[i][0] < SPACE_LEN) {
			vline.erase(vline.begin() + i + 1);
			i++;
		}
	}

	
	return lines.size() > 0;
}

bool CContractImg::ReadImg(string & imgPath)
{
	m_imgOrig = cv::imread(imgPath, cv::IMREAD_GRAYSCALE);

	if (m_imgOrig.empty()) {
		return false;
	}

	//去水印
	cv::threshold(m_imgOrig, m_imgBinary, 200, 255, cv::THRESH_BINARY);

	//边缘检测
	cv::Canny(m_imgBinary, m_imgPaint, 50, 200, 3);

	//初始化直线集合
	FindLines(m_imgPaint, m_horiLine, m_verLine);

	return true;
}
cv::Mat CContractImg::GetImgFromHoriLine(vector<cv::Vec4i>& lines, cv::Mat& orig, int l1, int l2)
{
	if (l1 > lines.size() || l2 > lines.size())
		throw("error");

	int left = lines[l1][0] < lines[l1][2] ? lines[l1][0] : lines[l1][2];
	int top = lines[l1][1];
	int right = lines[l2][0] > lines[l2][2] ? lines[l2][0] : lines[l2][2];
	int bottom = lines[l2][1];

	return cv::Mat(orig, cv::Rect(left, top, right - left, bottom - top));
}
string CContractImg::CustomName()
{
	//横线0，1之间
	
	cv::Mat nameImg = GetImgFromHoriLine(m_horiLine, m_imgBinary, 0, 1);




	return string();
}

string CContractImg::PhoneNum()
{
	//横线1，2之间
	cv::Mat phoneImg = GetImgFromHoriLine(m_horiLine, m_imgBinary, 1, 2);

	return string();
}

static vector<cv::RotatedRect> findTextRegion(cv::Mat img)
{
	vector<cv::RotatedRect> rects;
	//1.查找轮廓
	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(img, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//2.筛选那些面积小的
	for (int i = 0; i < contours.size(); i++)
	{
		//计算当前轮廓的面积
		double area = contourArea(contours[i]);

		//面积小于1000的全部筛选掉
		if (area < 1000)
			continue;

		//轮廓近似，作用较小，approxPolyDP函数有待研究
		double epsilon = 0.001*arcLength(contours[i], true);
		cv::Mat approx;
		cv::approxPolyDP(contours[i], approx, epsilon, true);

		//找到最小矩形，该矩形可能有方向
		cv::RotatedRect rect = minAreaRect(contours[i]);

		//计算高和宽
		int m_width = rect.boundingRect().width;
		int m_height = rect.boundingRect().height;

		//筛选那些太细的矩形，留下扁的
		if (m_height > m_width * 1.2)
			continue;

		//符合条件的rect添加到rects集合中
		rects.push_back(rect);

	}
	return rects;
}
static cv::Mat preprocess(cv::Mat gray)
{
	//1.Sobel算子，x方向求梯度
	cv::Mat sobel;
	cv::Sobel(gray, sobel, CV_8U, 1, 0, 3);

	//2.二值化
	cv::Mat binary;
	cv::threshold(sobel, binary, 0, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);

	//3.膨胀和腐蚀操作核设定
	cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(30, 9));
	//控制高度设置可以控制上下行的膨胀程度，例如3比4的区分能力更强,但也会造成漏检
	cv::Mat element2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(24, 4));

	//4.膨胀一次，让轮廓突出
	cv::Mat dilate1;
	cv::dilate(binary, dilate1, element2);

	//5.腐蚀一次，去掉细节，表格线等。这里去掉的是竖直的线
	cv::Mat erode1;
	cv::erode(dilate1, erode1, element1);

	//6.再次膨胀，让轮廓明显一些
	cv::Mat dilate2;
	cv::dilate(erode1, dilate2, element2);

	
	return dilate2;
}

string CContractImg::BusinessContent()
{
	//横线2，3之间
	cv::Mat businessImg = GetImgFromHoriLine(m_horiLine, m_imgBinary, 2, 3);
	
	cv::Mat dilate = preprocess(businessImg);

	vector<cv::RotatedRect> rects = findTextRegion(dilate);

	double maxArea = 0;

	//找面积最大的矩形
	cv::Rect maxrect;
	for each (cv::RotatedRect rect in rects)
	{
		//计算当前轮廓的面积
		double area = rect.size.width * rect.size.height;
		if (area > maxArea) {
			maxArea = area;
			maxrect = rect.boundingRect();
		}
	}

	businessImg = cv::Mat(businessImg, maxrect);
	
	//按行解析
	vector<cv::Rect> lines;
	int* shadow = new int[businessImg.rows];
	ZeroMemory(shadow, businessImg.rows * sizeof(int));

	//分割行
	for (int i = 0; i < businessImg.rows; i++) {
		for (int j = 0; j < businessImg.cols; j++) {
			if (businessImg.at<uchar>(i, j) == 0) {
				shadow[i] = 1;
				break;
			}
		}
	}
	

	bool bline = false;
	cv::Rect rectline;
	for (int i = 0; i < businessImg.rows; i++) {
		if (shadow[i] == 1 && !bline) {
			bline = true;
			rectline.x = 0;
			rectline.y = i;
			continue;
		}
		if (bline && shadow[i] == 0) {
			rectline.width = businessImg.cols;
			rectline.height = i - rectline.y;
			bline = false;
			lines.push_back(rectline);
		}
	}

	rectline.width = businessImg.cols;
	rectline.height = businessImg.rows - rectline.y;
	lines.push_back(rectline);

	vector<string> strlines;
	for each(cv::Rect rect in lines)
	{
		//cv::rectangle(businessImg, rect, cv::Scalar(0, 0, 0), 2);
		string strl = Img2String(cv::Mat(businessImg));
		strlines.push_back(strl);
	}


	delete[] shadow;

	return string();
}

string CContractImg::Img2String(cv::Mat)
{
	
	return string();
}
