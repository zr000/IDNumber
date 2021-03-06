
// IDNumberDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "IDNumber.h"
#include "IDNumberDlg.h"
#include "afxdialogex.h"

#include "CFileFilter.h"
#include "CCardImg.h"
#include "CContractImg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//https://max.book118.com/html/2017/1105/139015697.shtm

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIDNumberDlg 对话框

#define WM_USER_IMGPROCESS WM_USER + 100

CIDNumberDlg::CIDNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_IDNUMBER_DIALOG, pParent),
	m_thpool(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIDNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtl);
	DDX_Control(pDX, IDC_CMTHREAD, m_cbThread);
	DDX_Control(pDX, IDC_BTSTART, m_btStart);
	DDX_Control(pDX, IDC_PR, m_proNum);
	DDX_Control(pDX, ID_FOLDER, m_btOpen);
}

BEGIN_MESSAGE_MAP(CIDNumberDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_FOLDER, &CIDNumberDlg::OnBnClickedOpenFolder)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTSTART, &CIDNumberDlg::OnBnClickedBtstart)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CIDNumberDlg::OnLvnItemchangedListImages)
	ON_MESSAGE(WM_USER_IMGPROCESS, &CIDNumberDlg::OnUserImgprocess)
END_MESSAGE_MAP()


// CIDNumberDlg 消息处理程序

BOOL CIDNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	SetWindowPos(&CWnd::wndTop, 0, 0, 1024, 768, SWP_NOMOVE);

	CRect rect;
	GetWindowRect(rect);
	GetDlgItem(ID_FOLDER)->SetWindowPos(&CWnd::wndTop, 12, 10, 0, 0, SWP_NOSIZE);
	GetDlgItem(IDC_EDIT1)->SetWindowPos(&CWnd::wndTop, 120, 10, 0, 0, SWP_NOSIZE);
	
	m_cardArea.left = 120;
	m_cardArea.right = 1024 - 25;
	m_cardArea.top = 50;
	m_cardArea.bottom = 768 - 90;

	GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);

	CRect wndpos;
	GetDlgItem(IDC_EDIT1)->GetWindowRect(wndpos);

	m_cbThread.SetWindowPos(&CWnd::wndTop, wndpos.right, 12, 0, 0, SWP_NOSIZE);

	m_cbThread.GetWindowRect(wndpos);
	m_btStart.SetWindowPos(&CWnd::wndTop, wndpos.right + 70, 10, 0, 0, SWP_NOSIZE);

	m_btStart.GetWindowRect(wndpos);
	m_proNum.SetWindowPos(&CWnd::wndTop, wndpos.right + 10, 10, 0, 0, SWP_NOSIZE);

	UINT tn = std::thread::hardware_concurrency();
	char sznum[3];
	for (UINT i = 0; i < tn; i++) {
		_itoa_s(i+1, sznum, 10);
		m_cbThread.InsertString(i, sznum);

	}
	m_cbThread.SetCurSel(0);
	m_btStart.EnableWindow(FALSE);
	
	
	//界面绘制
	CDC* dc = GetDC();

	m_memDC.CreateCompatibleDC(dc);
	m_membmp.CreateCompatibleBitmap(dc, m_cardArea.Width(), m_cardArea.Height());

	m_memDC.SelectObject(m_membmp);

	//分割区域
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	m_memDC.SelectObject(GetStockObject(GRAY_BRUSH));
	m_memDC.SelectObject(pen);
	m_memDC.SetBkMode(TRANSPARENT);

	m_memDC.Rectangle(0, 0, m_cardArea.Width(), m_cardArea.Height());

	m_memDC.MoveTo(0, m_cardArea.Height() / 2);
	m_memDC.LineTo(m_cardArea.Width(), m_cardArea.Height() / 2);

	m_memDC.MoveTo(m_cardArea.Width() / 2, 0);
	m_memDC.LineTo(m_cardArea.Width() / 2, m_cardArea.Height());

	ReleaseDC(dc);

	int w = m_cardArea.Width() / 2 ;
	int h = m_cardArea.Height() / 2 ;

	m_fourArea[0] = CRect(2, 2, w - 2 , h - 2 );
	m_fourArea[1] = CRect(w + 2, 2, 2 * w - 2, h - 2);
	m_fourArea[2] = CRect(2, h + 2, w - 2, 2 * h - 2);
	m_fourArea[3] = CRect(w + 2, h + 2, 2 * w - 2, 2 * h - 2);

	m_listCtl.InsertColumn(0, "filename");
	m_listCtl.SetColumnWidth(0, 100);


	CContractImg cci;
	cci.ReadImg(string(R"(E:\Ultrapower\IDNumber\IDNumber\1-3.png)"));

	cci.CustomName();
	cci.PhoneNum();
	cci.BusinessContent();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIDNumberDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIDNumberDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		dc.SelectObject(GetStockObject(DEFAULT_GUI_FONT));
		
		CString strThread = "threads";
		CRect rect;
		m_cbThread.GetWindowRect(rect);
		ScreenToClient(rect);
		rect.left = rect.right + 2;
		rect.top = rect.top + 4;
		dc.SetBkMode(TRANSPARENT);
		dc.SelectObject(CPen(PS_SOLID, 1, RGB(0, 0, 0)));

		dc.DrawText(strThread, rect, DT_CALCRECT);
		dc.DrawText(strThread, rect, DT_LEFT);

		dc.BitBlt(m_cardArea.left, m_cardArea.top, m_cardArea.Width(), m_cardArea.Height(), &m_memDC, 0, 0, SRCCOPY);

		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIDNumberDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
 
static string OpenFolder()
{
	CFolderPickerDialog  dlg;
	
	if (IDOK == dlg.DoModal()) {

		return dlg.GetFolderPath().GetString();

	}

	return "";
}

void CIDNumberDlg::OnBnClickedOpenFolder()
{
	//CFileDialog cfd;
	CFileFilter cff;

	GetDlgItem(IDC_EDIT1)->SetWindowText(cff.GetOpenDirName(OpenFolder).c_str());

	cff.FilterDirFile();

	

	if (cff.GetFileCount() > 0) {
		m_btStart.EnableWindow();
	}
	else {
		return;
	}


	m_proNum.SetRange(0, cff.GetFileCount());
	m_proNum.SetStep(1);
	m_listCtl.DeleteAllItems();
	m_vecFiles.clear();

	string filePath = cff.GetNextFileName();

	while (filePath.length() > 0) {
		static int ni = 0;
		char szdriver[4];
		char szpath[MAX_PATH];
		char szname[MAX_PATH];
		char szext[MAX_PATH];

		_splitpath_s(filePath.c_str(), szdriver, szpath, szname, szext);

		m_listCtl.InsertItem(ni++, szname);
		m_vecFiles.push_back(filePath);

		filePath = cff.GetNextFileName();
	}

	cff.ResumeIndex();
}


void CIDNumberDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	
	CWnd* listwnd = GetDlgItem(IDC_LIST1);
	if (NULL != listwnd) {

		listwnd->SetWindowPos(&CWnd::wndTop, 10, 50, 105, cy - 60, SWP_NOZORDER);
	}
}

void CIDNumberDlg::ThreadImageProcess()
{
	while (true) {
		string strFullPath;

		m_lock.lock();

		if (m_ncurrProceFile < m_vecFiles.size()) {
			
			strFullPath = m_vecFiles[m_ncurrProceFile];
		}
		else {
			m_lock.unlock();
			break;
		}

		m_ncurrProceFile++;

		m_lock.unlock();

		ostringstream oss;
		oss << this_thread::get_id() << ":" << strFullPath.c_str() << endl;
		OutputDebugString(oss.str().c_str());
		
		Sleep(100);

		SendMessage(WM_USER_IMGPROCESS);
	}
}

void CIDNumberDlg::OnBnClickedBtstart()
{
	// TODO: 在此添加控件通知处理程序代码
	
	int threadnum = m_cbThread.GetCurSel() + 1;
	m_btStart.EnableWindow(FALSE);
	m_btStart.SetWindowText("running...");
	m_btOpen.EnableWindow(FALSE);

	if (m_thpool.size() > 0) {
		for (int i = 0; i < m_thpool.size(); i++) {
			m_thpool[i]->detach();
			delete m_thpool[i];
		}
	}
	m_thpool.clear();
	
	m_ncurrProceFile = 0;

	for (int i = 0; i < threadnum; i++) {
		thread* t = new thread(&CIDNumberDlg::ThreadImageProcess, this);
		m_thpool.push_back(t);
	}

	m_st = GetTickCount();

}


void CIDNumberDlg::OnLvnItemchangedListImages(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	
	if (pNMLV->uOldState == 0 && pNMLV->uNewState == (LVIS_SELECTED|LVIS_FOCUSED)) {
		string str;
		int nitem = pNMLV->iItem;
		str = m_vecFiles[nitem];

		CCardImg ci(str);
		if (ci.ReadCardImg()) {
			ci.DrawImg(ci.m_origImg, &m_memDC, m_fourArea[0]);
			CRect rect;
			rect.left = m_cardArea.left + m_fourArea[0].left;
			rect.top = m_cardArea.top + m_fourArea[0].top;
			rect.right = rect.left + m_fourArea[0].Width();
			rect.bottom = rect.top + m_fourArea[0].Height();

			InvalidateRect(rect);
		}
	}
	*pResult = 0;
}


afx_msg LRESULT CIDNumberDlg::OnUserImgprocess(WPARAM wParam, LPARAM lParam)
{
	m_proNum.StepIt();
	if (m_ncurrProceFile == m_vecFiles.size()) {
		m_btStart.EnableWindow();
		m_btStart.SetWindowText("start");
		m_btOpen.EnableWindow();

		DWORD sp = GetTickCount() - m_st;

		float fp = (float)m_vecFiles.size() / ((float)sp / 1000);
		CString strfp;
		strfp.Format("%.2f\n", fp);
		OutputDebugString(strfp);

		
	}
	return 0;
}
