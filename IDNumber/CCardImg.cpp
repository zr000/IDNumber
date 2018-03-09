#include "stdafx.h"
#include "CCardImg.h"


CCardImg::CCardImg(string strFullPath)
{
	if (strFullPath.length() > 0) {
		m_strFullPath = strFullPath;
	}
}


CCardImg::~CCardImg()
{
}


// 读取身份证图像
bool CCardImg::ReadCardImg()
{ 
	m_origImg = imread(m_strFullPath.c_str(), IMREAD_COLOR);
	
	if (NULL == m_origImg.data)
		return false;

	return true;
}


// 设置绘图dc
void CCardImg::SetPaintDC(CDC* paintDC)
{
	if (NULL != paintDC)
		m_paintDC = paintDC;
}
