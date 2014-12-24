
// CodeTesterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CodeTester.h"
#include "CodeTesterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCodeTesterDlg 对话框




CCodeTesterDlg::CCodeTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCodeTesterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hSelectedItem = NULL;
	m_nCaseCount = 0;
	m_nRunCaseCursor = 0;
	m_hCurrentItem = NULL;
//	m_pTesterCm = NULL;
}

void CCodeTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_TESECASE, m_TestCaseTreeCtrl);
	DDX_Control(pDX, IDC_LOG_CTRL, m_OutPutCtrl);
	DDX_Control(pDX, IDC_RADIO_RUNLED, m_RadioRunLedCtrl);
	DDX_Control(pDX, IDC_RADIO_RUNLED2, m_RadioLed1Ctrl);
	DDX_Control(pDX, IDC_RADIO_RUNLED3, m_RadioBlueLedCtrl);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_InputEditCtrl);
	DDX_Control(pDX, IDC_COMBO_INPUT, m_CmbInput);
}

BEGIN_MESSAGE_MAP(CCodeTesterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_WM_CONTEXTMENU()

	ON_NOTIFY(NM_DBLCLK, IDC_TREE_TESECASE, &CCodeTesterDlg::OnNMMDblclkCaseTreeCtrl)

#if 0
	//ON_NOTIFY(NM_CLICK, IDC_TREE_HW, &CCodeTesterDlg::OnNMClickHwTreeCtrl)
	//ON_NOTIFY(NM_CLICK, IDC_TREE_CM, &CCodeTesterDlg::OnNMClickCmTreeCtrl)	ON_COMMAND(ID_RUN, OnRunTest)
	ON_COMMAND(ID_SETUP, OnSetUp)
	ON_BN_CLICKED(IDRESET, &CEmmcTesterDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDCLOSE, &CEmmcTesterDlg::OnBnClickedClose)
	ON_MESSAGE(ID_CASEDONE, &CEmmcTesterDlg::OnCaseDone)
	ON_MESSAGE(ID_UPDATE_TREE, &CEmmcTesterDlg::OnUpdateDeviceTree)
	ON_COMMAND(ID_RUN_NORMAL, &CEmmcTesterDlg::OnRunNormal)
	ON_COMMAND(ID_STOP, &CEmmcTesterDlg::OnStop)
	ON_COMMAND(ID_WATCHDOG_RESET, &CEmmcTesterDlg::OnWatchDogReset)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CEmmcTesterDlg::OnNMDblclkTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE2, &CEmmcTesterDlg::OnNMClickTree2)
	ON_NOTIFY(TVN_ITEMCHANGED, IDC_TREE2, &CEmmcTesterDlg::OnTvnItemChangedTree2)
	ON_CBN_SELCHANGE(IDC_SITENUMBER, &CEmmcTesterDlg::OnCbnSelchangeSitenumber)
	ON_BN_CLICKED(IDC_INPUT, &CEmmcTesterDlg::OnBnClickedInput)
#endif
	ON_BN_CLICKED(IDC_INPUT, &CCodeTesterDlg::OnBnClickedInput)
	ON_BN_CLICKED(IDRESET, &CCodeTesterDlg::OnBnClickedReset)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_SAVE, &CCodeTesterDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(ID_BTN_DEL, &CCodeTesterDlg::OnBnClickedBtnDel)
	ON_CBN_SELCHANGE(IDC_COMBO_INPUT, &CCodeTesterDlg::OnCbnSelchangeComboInput)
END_MESSAGE_MAP()


// CCodeTesterDlg 消息处理程序

BOOL CCodeTesterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	/*
	m_pTesterCm = new CTesterCm();
	g_pTesterCm = m_pTesterCm;
	*/
	m_TestCaseTreeCtrl.Init();
	m_FileViewImages.Create(IDB_BITMAP1, 16, 0, RGB(255, 255, 255));
	m_TestCaseTreeCtrl.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
	m_TestCaseTreeCtrl.FillTreeNodeData(g_TestCaseTreeData);

	//m_InputEditCtrl.SetWindowTextW(L"CallStart(\"1234\",\"192.168.0.27\")");//SocketSendTo(0xC0A80019, 2013, 0X1234)
	//m_InputEditCtrl.SetWindowTextW(L"MsgSectionInit(\"192.168.0.27\",0x00010001, 0x04, 0x05)");//SocketSendTo(0xC0A80019, 2013, 0X1234)

	WCHAR buff[100];
	CString keyName;
	int i = 0;
	for(i = 1; i < 100; i++)
	{
		keyName.Format(L"Cmd%02d", i);

		::GetPrivateProfileString(L"CmdList",keyName,L"0", buff, 100, theApp.m_ProfileName);
		if(buff[0] == L'0')
		{
			break;
		}
		m_CmbInput.AddString(buff);
	}

	//如果没有则配置默认值
	if(i == 1)
	{
		m_CmbInput.AddString(L"?\r");
	}

	int nCurSel = 0;
	nCurSel = ::GetPrivateProfileInt(L"ComboImput",L"CurSel", 0, theApp.m_ProfileName);
	m_CmbInput.SetCurSel(nCurSel);
	// TODO: 在此添加额外的初始化代码
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCodeTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCodeTesterDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCodeTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCodeTesterDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd == (CWnd*)&m_TestCaseTreeCtrl)
	{
		m_TestCaseTreeCtrl.OnContextMenu(point);
	}
}

HTREEITEM CCodeTesterDlg::GetSelectedItem(CTreeCtrl* pWndTree, UINT& flags)
{
	HTREEITEM hItem = NULL;
	//TRACE(L"%s()\n", _FUNC_);

	ASSERT_VALID(pWndTree);
	DWORD dwPos = GetMessagePos();
	CPoint point(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
	
	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		pWndTree->ScreenToClient(&point);

		hItem = pWndTree->HitTest(point, &flags);

		//TRACE(L"%s() flags=%d\n", _FUNC_, flags);
		if(flags & TVHT_ONITEMLABEL || flags & TVHT_ONITEMSTATEICON)
		{
			if (hItem != NULL)
			{
				pWndTree->SelectItem(hItem);
			}
		}
		else
		{
			return NULL;
		}
	}

	return hItem;
}

void CCodeTesterDlg::OnNMMDblclkCaseTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UINT flags = 0;
	HTREEITEM hItem = GetSelectedItem(&m_TestCaseTreeCtrl, flags);

	if(hItem)
	{
		m_TestCaseTreeCtrl.OnDbClick(hItem);
	}
}

void CCodeTesterDlg::SendUdp(BYTE* pData, int len) 
{
	// TODO: Add your control notification handler code here
	//AfxSocketInit( NULL);

	CSocket echoClient;  

	if (echoClient.Create(0,SOCK_DGRAM,NULL) == 0) 
	{
		MessageBox(L"Create() failed");
	}

	if (echoClient.SendTo(pData, len, 16, L"localhost", 0) != len) 
	{
		MessageBox(L"SendTo() sent a different number of bytes than expected");
	}

	echoClient.Close();

}

void CCodeTesterDlg::OnBnClickedInput()
{
	static BOOL cmdLineInitFlag = FALSE;
	if(!cmdLineInitFlag)
	{
		//初始化Shell
		Shell_Init();
		cmdLineInitFlag = TRUE;
	}

	CString strWindowText;
	m_CmbInput.GetWindowTextW(strWindowText);
	strWindowText += L"\r";

	CStringA strA(strWindowText);
	char* pChar = strA.GetBuffer(strA.GetLength());

	CmdLine_AddStr(pChar);
}


void CCodeTesterDlg::OnBnClickedReset()
{
	m_OutPutCtrl.Reset();
}


void CCodeTesterDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if(m_pTesterCm)
	//	delete m_pTesterCm;
	CDialogEx::OnClose();
}

BOOL CCodeTesterDlg::isExist(const CString& dstStr)
{
	CString keyName;

	for(int i = 0; i < m_CmbInput.GetCount(); i++)
	{
		CString str;
		keyName.Format(L"Cmd%02d", i+1);

		m_CmbInput.GetLBText(i, str);
		if(dstStr == str)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CCodeTesterDlg::OnSave()
{
	CString keyName;

	for(int i = 0; i < m_CmbInput.GetCount(); i++)
	{
		CString str;
		keyName.Format(L"Cmd%02d", i+1);

		m_CmbInput.GetLBText(i, str);
		::WritePrivateProfileString(L"CmdList",keyName, str, theApp.m_ProfileName);
	}
}

void CCodeTesterDlg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strWindowText;
	m_CmbInput.GetWindowTextW(strWindowText);

	if(!isExist(strWindowText))
	{
		m_CmbInput.AddString(strWindowText);
	}

	OnSave();
}


void CCodeTesterDlg::OnBnClickedBtnDel()
{
	int n = m_CmbInput.GetCurSel();

	if(n == -1) return;

	m_CmbInput.DeleteString(n);

	if(m_CmbInput.GetCount() > n)
	{
	}
	else if(n -1 > 0)
	{
		n -= 1;
	}
	else
	{
		n = 0; 
	}

	m_CmbInput.SetCurSel(n);

	//OnSave();
}


void CCodeTesterDlg::OnCbnSelchangeComboInput()
{
	CString str;
	int n = m_CmbInput.GetCurSel();
	if(n == -1) return;

	str.Format(L"%d",n);

	::WritePrivateProfileString(L"ComboImput",L"CurSel", str, theApp.m_ProfileName);
}
