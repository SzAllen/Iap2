
// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MainApp.h"
#include "afxdialogex.h"
#include "MainDlg.h"
#include "Dbt.h"
#include "SheetSettings.h"
#include "DlgUartTester.h"
#include "DlgIap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LPCTSTR g_pComSettingSection = L"ComSetting";
LPCTSTR g_pComSetting[] = {
	 L"Baudrate"
	,L"Databits"
	,L"Parity"
	,L"Stopbits"
	,L"Handshaking"

	,L"EventTypes"
	,L"EventCharacter"
};

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


// CMainDlg 对话框




CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainDlg::IDD, pParent)
{
	Hander = NULL;
	m_strComPort = L"COM1";
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_TxDataFlag = UART_FLAG_TX;
	m_TimerId = 0;
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tabctrl);
	DDX_Control(pDX, IDC_RICHEDIT21, m_LogCtrl);
	DDX_Control(pDX, IDC_PROGRESS_DOWNLOAD, m_ProgCtrl);
	DDX_Control(pDX, IDC_COM_NUM, m_ComListCtrl);

	DDX_Control(pDX, IDC_CHK_SHOW_TX, m_ChkShowTx);
	DDX_Control(pDX, IDC_CHK_SHOW_RX, m_ChkShowRx);
	DDX_Control(pDX, IDC_STATIC_COM, m_ComCfgLog);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()

	ON_MESSAGE(WM_DEVICECHANGE, OnMyDeviceChange)
	ON_MESSAGE(WM_LOG		,&CMainDlg::OnLog)
	ON_MESSAGE(WM_DOWNLOAD	,&CMainDlg::OnDownload)
	ON_MESSAGE(WM_DLG_INIT	,&CMainDlg::OnDlgInit)
//	ON_MESSAGE(WM_TX_DATA	, &CMainDlg::OnTxData)

	ON_BN_CLICKED(ID_BTN_CLEAN, &CMainDlg::OnBnClickedBtnClean)
	ON_WM_SERIAL(OnSerialMsg)
	ON_BN_CLICKED(IDC_BTN_SETTING, &CMainDlg::OnBnClickedBtnSetting)
	ON_CBN_SELCHANGE(IDC_COM_NUM, &CMainDlg::OnCbnSelchangeComNum)
	ON_BN_CLICKED(IDC_CHK_SHOW_TX, &CMainDlg::OnBnClickedChkShowTx)
	ON_BN_CLICKED(IDC_CHK_SHOW_RX, &CMainDlg::OnBnClickedChkShowRx)
	ON_BN_CLICKED(IDOK, &CMainDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CMainDlg 消息处理程序
BOOL CMainDlg::OnInitDialog()
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

	// TODO: 在此添加额外的初始化代码
	m_imagelistTab.Create(IDB_TABCTRL_IMAGE, 11, 0, RGB(0, 255, 0));
	m_tabctrl.SetImageList(&m_imagelistTab);
	
	m_tabctrl.SetDisabledColor(RGB(255, 0, 0));
	m_tabctrl.SetSelectedColor(RGB(0, 0, 255));
	m_tabctrl.SetMouseOverColor(RGB(255, 255, 255));
	
	int isEnable = FALSE;
	
	m_tabctrl.AddTabDlg(new CDlgUartTester(this), L"Uart Transfer Tester", 0);
	m_tabctrl.AddTabDlg(new CDlgIap(this), L"Iap2 Tester", 0);

	if(m_tabctrl.GetItemCount() > 0)
	{
		m_tabctrl.EnableTab(0, TRUE);
	}
	int tabIndex = g_Profile.ReadInt(L"Setting", L"TabIndex", 0);
	if(tabIndex >= m_tabctrl.GetItemCount())
	{
		tabIndex = 0;
	}
	m_tabctrl.SelectTab(tabIndex);

	// Open the serial port
	CStringArray nameList;
	CSerial::GetSystemPorts(m_ComPortList, &nameList);
	for(int i = 0; i < m_ComPortList.GetCount(); i++)
	{
		m_ComListCtrl.AddString(nameList[i]);
	}
	if(m_ComListCtrl.GetCount())
	{
		m_ComListCtrl.Init(&g_Profile, L"ComSetting", L"ComNum", m_ComPortList[0]);
	}

	PostMessage(WM_DLG_INIT);

	m_ChkShowTx  .Init(&g_Profile, L"Setting", L"ShowTx", L"0");
	m_ChkShowRx  .Init(&g_Profile, L"Setting", L"ShowRx", L"0");

	CRect Rect;
	GetWindowRect(&Rect);
	//SetWindowPos(NULL, Rect.left, Rect.top, Rect.right - Rect.left, Rect.top + 285,  SWP_DRAWFRAME|SWP_NOMOVE); 
	GetDlgItem(IDC_BTN_SETTING)->ShowWindow(FALSE);
	//GetDlgItem(IDC_STATIC_COM)->ShowWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CMainDlg::OpenCom()
{
	int eBaudrate,eDataBits,eParity,eStopBits;
	eBaudrate = g_Profile.ReadInt(g_pComSettingSection, g_pComSetting[Baudrate], CSerial::EBaud115200);
	eDataBits = g_Profile.ReadInt(g_pComSettingSection, g_pComSetting[Databits], CSerial::EData8);
	eParity   = g_Profile.ReadInt(g_pComSettingSection, g_pComSetting[Parity]  , CSerial::EParOdd);
	eStopBits = g_Profile.ReadInt(g_pComSettingSection, g_pComSetting[Stopbits], CSerial::EStop1);

	//m_strComPort = m_ComListCtrl.GetString();
	m_strComPort = m_ComPortList[m_ComListCtrl.GetCurSel()];

	CString comNum = m_strComPort;
	CSerial::FormatComPort(comNum);

	if (m_Serial.Open(comNum,this) != ERROR_SUCCESS)
	{
		DWORD dwError = GetLastError();
		TRACE(L"error=%d\n",dwError);

		CString str;
		str.Format(_T("%s Open failed.\n"), m_strComPort);
		m_LogCtrl.Log(str);
		//AfxMessageBox(str,MB_ICONSTOP|MB_OK);
		//PostMessage(WM_CLOSE);
		GetDlgItem(IDC_STATIC_COM)->SetWindowTextW(str);
		GetDlgItem(IDC_BTN_SETTING)->EnableWindow(FALSE);
		Notify(EVENT_USB_PLUG, FALSE);
		return;
	}
	Notify(EVENT_USB_PLUG, TRUE);
	GetDlgItem(IDC_BTN_SETTING)->EnableWindow(TRUE);

	if(m_Serial.Setup((CSerial::EBaudrate)eBaudrate
		,(CSerial::EDataBits)eDataBits
		,(CSerial::EParity)eParity
		,(CSerial::EStopBits)eStopBits))
	{
		CString str;
		str.Format(_T("%s setting failed."), m_strComPort);
		m_LogCtrl.Log(str);
		//AfxMessageBox(str, MB_ICONSTOP|MB_OK);
	}
	UpdateComUI();

	m_LogCtrl.Log(L"Open %s Success.\n", m_strComPort);
}

LRESULT CMainDlg::OnDlgInit(UINT nID, LPARAM lParam)
{
	OpenCom();
	return 0;
}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMainDlg::OnPaint()
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
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMainDlg::OnDestroy() 
{
	m_Serial.Close();
	m_tabctrl.DeleteAllTabs();
	CDialogEx::OnDestroy();
}

void CMainDlg::Log(LPCTSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	wchar_t pszBuffer[1024];
#pragma warning (push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)
	vswprintf( pszBuffer, 1024, pszFormat, ptr );
#pragma warning (pop)
	va_end(ptr);

	m_LogCtrl.Log(pszBuffer);

}

void CMainDlg::Log(const char* pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	char pszBuffer[1024];
#pragma warning (push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)
	vsprintf( pszBuffer, pszFormat, ptr );
#pragma warning (pop)
	va_end(ptr);

	m_LogCtrl.Log(pszBuffer);
}

void CMainDlg::Reset()
{
	m_LogCtrl.Reset();
}

void CMainDlg::Notify(EVENT eEvent, WPARAM param, LPARAM lparam)
{
	for(int i = 0; i < m_tabctrl.GetItemCount(); i++)
	{
		CTabDlg* pDlg = m_tabctrl.GetTableItemEx(i);
		pDlg->OnNotify(eEvent, param, lparam);
	}
}
LRESULT CMainDlg::OnLog(WPARAM wparam, LPARAM lp)
{
	if(lp)
	{
		Log((char*)wparam);
	}
	else
	{
		Log((LPCTSTR)wparam);
	}

	return 0;
}

LRESULT CMainDlg::OnDownload(WPARAM wparam, LPARAM pos)
{
	BOOL bFlag = FALSE;
	Notify(EVENT_DOWNLOAD_PROCESS, wparam, pos);
	if(DOWNLOAD_START == wparam)
	{
		m_ProgCtrl.SetRange(0, (short)pos);
		m_ProgCtrl.ShowWindow(1);
		m_ProgCtrl.SetPos(0);
	}
	else if(DOWNLOAD_END == wparam)
	{
		m_ProgCtrl.ShowWindow(0);
	}
	else if(DOWNLOAD_POS == wparam)
	{
		m_ProgCtrl.SetPos(pos);
	}

	return 0;
}

void CMainDlg::OnBnClickedBtnClean()
{
	m_LogCtrl.Reset();
}

void CMainDlg::OnBnClickedBtnSetting()
{
	CSheetSettings sheetSettings(&m_Serial,this);
	sheetSettings.DoModal();
	UpdateComUI();
}

void CMainDlg::OnCbnSelchangeComNum()
{
	if(m_strComPort == m_ComListCtrl.GetString()) return;

	m_Serial.Close();
	m_LogCtrl.Log(L"Close %s.\n", m_strComPort);

	OpenCom();

	m_ComListCtrl.Save();
}

void CMainDlg::UpdateComUI()
{
	CSerial::EBaudrate  eBaudrate  = m_Serial.GetBaudrate();
	CSerial::EDataBits  eDataBits  = m_Serial.GetDataBits();
	CSerial::EParity    eParity    = m_Serial.GetParity();
	CSerial::EStopBits  eStopBits  = m_Serial.GetStopBits();
	CSerial::EHandshake eHandshake = m_Serial.GetHandshaking();

	CString str;
	str.Format(L"<P align=center><font size=3>Baudrate:[<b>%d</b>]; Databits:[<b>%d</b>]; Parity:[<b>%s</b>]; Stopbits:[<b>%s</b>]</font></p>"
		, eBaudrate
		, eDataBits
		, m_ParityStr[eParity]
		, m_StopBitsStr[eStopBits]);

	str.Format(L"Baudrate:[%d]; Databits:[%d]; Parity:[%s]; Stopbits:[%s]"
		, eBaudrate
		, eDataBits
		, m_ParityStr[eParity]
		, m_StopBitsStr[eStopBits]);


	//m_ComCfgLog.SetScript(str);
	GetDlgItem(IDC_STATIC_COM)->SetWindowTextW(str);
}

void CMainDlg::DisplayEvent (LPCTSTR lpszMsg)
{
	/*
	// Format the selection as red text
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR;
	m_wndEdit.GetSelectionCharFormat(cf);
	cf.crTextColor = 0x000000FF;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	m_wndEdit.SetSelectionCharFormat(cf);

	// Display the text
	TCHAR tszMsg[200];
	wsprintf(tszMsg,_T("[%s]"), lpszMsg);
	m_wndEdit.SetSel(-1,-1);
	m_wndEdit.ReplaceSel(tszMsg);
	*/
}

void CMainDlg::DisplayEventSetting (LPCTSTR lpszMsg, LPCTSTR lpszSetting, bool fOn)
{
	/*
	TCHAR tszMsg[200];
	wsprintf(tszMsg,_T("%s (%s=%s)"), lpszMsg, lpszSetting, (fOn?_T("on"):_T("off")));
	DisplayEvent(tszMsg);*/
}

LRESULT CMainDlg::OnSerialMsg (WPARAM wParam, LPARAM /*lParam*/)
{
	CSerial::EEvent eEvent = CSerial::EEvent(LOWORD(wParam));
	CSerial::EError eError = CSerial::EError(HIWORD(wParam));

	if (eError)
		DisplayEvent(_T("An internal error occurred."));

	if (eEvent & CSerial::EEventBreak)
		DisplayEvent(_T("Break detected on input."));

	if (eEvent & CSerial::EEventError)
		DisplayEvent(_T("A line-status error occurred."));
	
	if (eEvent & CSerial::EEventRcvEv)
		DisplayEvent(_T("Event character has been received."));

	if (eEvent & CSerial::EEventRing)
		DisplayEvent(_T("Ring detected"));
	
	if (eEvent & CSerial::EEventSend)
		DisplayEvent(_T("All data is send"));
	
	if (eEvent & CSerial::EEventCTS)
		DisplayEventSetting(_T("CTS signal change"), _T("CTS"), m_Serial.GetCTS());
	
	if (eEvent & CSerial::EEventDSR)
		DisplayEventSetting(_T("DSR signal change"), _T("DSR"), m_Serial.GetDSR());
	
	if (eEvent & CSerial::EEventRLSD)
		DisplayEventSetting(_T("RLSD signal change"), _T("RLSD"), m_Serial.GetRLSD());
	
	if (eEvent & CSerial::EEventRecv)
	{
		// Create a clean buffer
		DWORD dwRead;
		BYTE szData[101];

		const int nBuflen = sizeof(szData)-1;

		// Obtain the data from the serial port
		do
		{
			m_Serial.Read(szData,nBuflen,&dwRead);

			if(m_ChkShowRx.GetCheck())
			{
				if(UART_FLAG_TX == m_TxDataFlag)
				{
					Log("Rx: ");
					m_TxDataFlag = UART_FLAG_RX;
				}

				for(DWORD i = 0; i < dwRead; i++)
				{
					Log("%02X ", szData[i]);
				}
			}

			if(dwRead && Hander)
			{
				Hander(m_pHanderArg, szData, dwRead);
			}
			
		}while(dwRead);
	}

	return 0;
}

LRESULT CMainDlg::OnTxData(const void* pData, UINT len)
{
	BYTE* pByte = (BYTE*)pData;

	if(UART_FLAG_RX == m_TxDataFlag)
	{
		Printf("\n");
	}

	if(m_ChkShowTx.GetCheck())
	{
		Printf("Tx: ");
		DUMP_BYTE(pByte, len);
	}

	m_TxDataFlag = UART_FLAG_TX;
	m_Serial.Write(pByte, len);
	return 0;
}


void CMainDlg::OnBnClickedChkShowTx()
{
	m_ChkShowTx.Save();
}

void CMainDlg::OnBnClickedChkShowRx()
{
	m_ChkShowRx.Save();
}

BOOL CMainDlg::RegsterRcvDataCb(CTabDlg* pDlg, HandlerFn rcvHandler)
{
	if(NULL == Hander || NULL == rcvHandler)
	{
		Hander = rcvHandler;
		m_pHanderArg = pDlg;
		return TRUE;
	}

	return FALSE;
}


void CMainDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}

LRESULT CMainDlg::OnMyDeviceChange(WPARAM wParam, LPARAM lParam)
{
	if(DBT_DEVTYP_DEVINST == wParam ) 
	{
		OnDeviceChange();
	}
	return 0;
}

void CMainDlg::OnDeviceChange()
{
	int nIndex = CB_ERR;
	CStringArray comList;
	CStringArray nameList;
	CSerial::GetSystemPorts(comList, &nameList);

	if(comList.GetCount() != m_ComPortList.GetCount())
	{
		m_ComPortList.RemoveAll();
		m_ComPortList.Copy(comList);
		m_ComListCtrl.ResetContent();

		for(int i = 0; i < m_ComPortList.GetCount(); i++)
		{
			m_ComListCtrl.AddString(nameList[i]);
			if(m_strComPort == m_ComPortList[i])
			{
				nIndex = i;
			}
		}

		if(CB_ERR == nIndex)
		{
			if(m_ComPortList.GetCount())
			{
				CString str; 
				if(m_strComPort.GetLength())
				{
					m_Serial.Close();
					m_strComPort = L"";
				}
				str.Format(L"Please selece a serial com.");
				m_ComCfgLog.SetWindowText(str);
			}
		}
		else
		{
			m_ComListCtrl.SetCurSel(nIndex);
		}
	}
}

void CMainDlg::OnTimer(UINT nIDEvent) 
{
}
