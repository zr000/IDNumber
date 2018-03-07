#pragma once
#include <string>
#include <vector>
#include <regex>


using namespace std;

typedef string(*OpenDir)(void);

class CFileFilter
{
public:
	CFileFilter();
	virtual ~CFileFilter();

protected:
	// 打开的文件夹
	string m_strDir;
	// 过滤后的文件名
	vector<string> m_vecFiles;
	// 过滤规则
	string m_strRule;
	// 打开文件函数
	OpenDir pfOpenDir;

	virtual bool _filter(string content);

	size_t m_nFileIndex;
public:
	// 获取文件夹路径
	void GetOpenDirName(OpenDir func);

	// 按照规则过滤文件夹里的文件
	virtual int FilterDirFile();
	
	string GetNextFileName();
	void ResumeIndex();
};

