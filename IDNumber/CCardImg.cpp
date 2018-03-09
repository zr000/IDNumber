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
	
	if (m_origImg.empty())
		return false;

	m_imgSize.cx = m_origImg.cols;
	m_imgSize.cy = m_origImg.rows;


	return true;
}


#ifdef _WIN32

void CCardImg::MatToCImage(Mat &mat, CImage &cImage)
{
	int width = mat.cols;
	int height = mat.rows;
	int channels = mat.channels();

	cImage.Destroy();
	cImage.Create(width,
		height,
		8 * channels);

	uchar* ps;
	uchar* pimg = (uchar*)cImage.GetBits();
	int step = cImage.GetPitch();

	for (int i = 0; i < height; ++i)
	{
		ps = (mat.ptr<uchar>(i));
		for (int j = 0; j < width; ++j)
		{
			if (channels == 1) //gray    
			{
				*(pimg + i * step + j) = ps[j];
			}
			else if (channels == 3) //color    
			{
				for (int k = 0; k < 3; ++k)
				{
					*(pimg + i * step + j * 3 + k) = ps[j * 3 + k];
				}
			}
		}
	}

}

void CCardImg::DrawImg(cv::Mat& mat, CDC * dc, CRect rect)
{
	if (mat.empty() || mat.depth() != CV_8U)
		return;

	cv::Mat matdraw;

	cv::resize(mat, matdraw, cv::Size(rect.Width(), rect.Height()));

	CImage imgdraw;

	MatToCImage(matdraw, imgdraw);

	imgdraw.Draw(dc->GetSafeHdc(), rect);
}
#endif // _WIN32


