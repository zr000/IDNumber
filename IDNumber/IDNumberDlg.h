
// IDNumberDlg.h: 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

// CIDNumberDlg 对话框
class CIDNumberDlg : public CDialog
{
// 构造
public:
	CIDNumberDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IDNUMBER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	CDC m_memDC;
	CBitmap m_membmp;
	CRect m_cardArea;
	CRect m_fourArea[4];
	

	vector<thread*> m_thpool;
	vector<string> m_vecFiles;
	std::atomic_int m_ncurrProceFile;
	
	std::mutex m_lock;

	DWORD m_st;
	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	

	afx_msg void OnBnClickedOpenFolder();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListCtrl m_listCtl;
	// 选择线程数
	CComboBox m_cbThread;
	// 开始执行
	CButton m_btStart;
	// 进度条
	CProgressCtrl m_proNum;
	afx_msg void OnBnClickedBtstart();
	CButton m_btOpen;
	afx_msg void OnLvnItemchangedListImages(NMHDR *pNMHDR, LRESULT *pResult);

	void ThreadImageProcess();
protected:
	afx_msg LRESULT OnUserImgprocess(WPARAM wParam, LPARAM lParam);
};
