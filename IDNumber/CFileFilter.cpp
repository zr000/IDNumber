#include "stdafx.h"

#include "CFileFilter.h"


CFileFilter::CFileFilter():
pfOpenDir(NULL),
m_nFileIndex(0)
{
}


CFileFilter::~CFileFilter()
{
}

bool CFileFilter::_filter(string content)
{
	regex regfile(".*\\.(png|bmp|jpeg|jpg)");
	
	if (regex_match(content.c_str(), regfile))
		return true;

	return false;
}

void CFileFilter::GetOpenDirName(OpenDir func)
{
	if (NULL == func)
		return;

	pfOpenDir = func;

	m_strDir = func();
}

int CFileFilter::FilterDirFile()
{
	//string m_strDir = R"(E:\zrapp\part_time\part_time\upload\res)";

	if (m_strDir.length() > 0) {
		
		struct _finddata_t fileinfo;

		string strfullPath = m_strDir + "/*.*";

		long long hfind;
		
		if ((hfind = _findfirst(strfullPath.c_str(), &fileinfo)) == -1)
			return 0;
		
		do {
			if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0)
				continue;

			if (_filter(fileinfo.name)) {
				m_vecFiles.push_back(m_strDir + "/" + fileinfo.name);
			}

		} while (_findnext(hfind, &fileinfo) == 0);
		
		_findclose(hfind);
	}

	return m_vecFiles.size();
}

string CFileFilter::GetNextFileName()
{
	if (m_nFileIndex < m_vecFiles.size())
		return m_vecFiles[m_nFileIndex++];
	else
		return "";
}

void CFileFilter::ResumeIndex()
{
	m_nFileIndex = 0;
}

