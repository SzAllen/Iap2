// DlgUartTester.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgUartTester.h"
#include "afxdialogex.h"
#include "MainDlg.h"


// CDlgUartTester 对话框
const static BYTE g_TestDataSrc[TEST_DATA_SIZE]	= {0x5A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
static BYTE g_TestDataDst[TEST_DATA_SIZE]			= {0xA5};

IMPLEMENT_DYNAMIC(CDlgUartTester, CTabDlg)

CDlgUartTester::CDlgUartTester(CLog *pLog, CWnd* pParent /*=NULL*/)
	: CTabDlg(pLog, CDlgUartTester::IDD, pParent)
	, m_nMaxReSendCount(0)
{
	for(int i = 1; i < sizeof(g_TestDataSrc); i++)
	{
		g_TestDataDst[i] = g_TestDataSrc[TEST_DATA_SIZE - i];
	}
	m_TimerId = 0;
	TestReset();
}

CDlgUartTester::~CDlgUartTester()
{
}

void CDlgUartTester::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LOG, m_TestResultCtrl);
	//DDX_Control(pDX, IDC_RESEND_COUNT, m_TestResultCtrl);
	DDX_Control(pDX, IDC_RESEND_COUNT, m_ReSendCountCtrl);
	DDX_Text(pDX, IDC_RESEND_COUNT, m_nMaxReSendCount);
	DDV_MinMaxByte(pDX, m_nMaxReSendCount, 1, 255);
}


BEGIN_MESSAGE_MAP(CDlgUartTester, CTabDlg)
	ON_BN_CLICKED(IDC_UART_TEST_START, &CDlgUartTester::OnBnClickedUartTestStart)
	ON_WM_TIMER()
	ON_MESSAGE(WM_RESEND, &CDlgUartTester::OnReSend)
	ON_EN_CHANGE(IDC_RESEND_COUNT, &CDlgUartTester::OnEnChangeResendCount)
END_MESSAGE_MAP()

BOOL CDlgUartTester::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ReSendCountCtrl.Init(&g_Profile, L"Setting", L"ReSendCount", L"20");
	m_nMaxReSendCount = m_ReSendCountCtrl.GetValue();
	return TRUE;
}

void CDlgUartTester::OnBnClickedUartTestStart()
{
	CMainDlg* pDlg = (CMainDlg*)AfxGetMainWnd();
	if(!pDlg->RegsterRcvDataCb(this, (HandlerFn)RxDataHandler))
	{
		MessageBox(L"Uart is not ready.", L"警告", MB_OK|MB_ICONWARNING);
		return;
	}
	m_pLog->Reset();
	m_TestResultCtrl.ShowWindow(FALSE);
	UartTestStart();
}

void CDlgUartTester::UartTestStart()
{
	if(0 == m_TimerId)
	{
		m_pLog->Log("Uart test start...\n");
		m_TimerId = SetTimer(0, 200, NULL);
		UpdateProcessCtrl(DOWNLOAD_START, m_nMaxReSendCount);
		return;
	}
	else
	{
		m_nReTxCounter++;
		UpdateProcessCtrl(DOWNLOAD_POS, m_nReTxCounter+2);
	}

	CMainDlg* pDlg = (CMainDlg*)AfxGetMainWnd();
	pDlg->OnTxData(g_TestDataSrc, sizeof(g_TestDataSrc));
}

void CDlgUartTester::OnTimer(UINT nIDEvent)
{
	CTabDlg::OnTimer(nIDEvent);

	if(m_TimerId)
	{
		PostMessage(WM_RESEND, 0, 0);
	}
}

LRESULT CDlgUartTester::OnReSend(WPARAM wParam, LPARAM lParam)
{
	if(m_nReTxCounter >= m_nMaxReSendCount)
	{
		TestReset();
		m_pLog->Log("Uart test: FAILED.\n");
		m_TestResultCtrl.ShowWindow(TRUE);
		m_TestResultCtrl.SetScript(L"<P align=center><font face=verdana size=7 color=\"white\"><b>FAILED</b></font></p>");
		m_TestResultCtrl.SetBackgroundColour(RGB(255,0,0));
		UpdateProcessCtrl(DOWNLOAD_END, 0);
		MessageBox(L"Uart 测试失败", L"警告", MB_OK|MB_ICONWARNING);
		GetDlgItem(IDC_UART_TEST_START)->SetFocus();
	}
	else
	{
		UartTestStart();
	}
	return 0;
}

// CDlgUartTester 消息处理程序
void CDlgUartTester::TestReset()
{
	m_nReTxCounter = 0;
	m_nErrCounter = 0;
	m_nLen = 0;
	memset(m_dstData, 0, sizeof(m_dstData));
	m_isFoundHead = FALSE;

	if(m_TimerId)
	{
		KillTimer(m_TimerId);
		m_TimerId = 0;
	}
}

void CDlgUartTester::RxDataHandler(CDlgUartTester* pDlg, const uint8* pData, int nDataLen)
{
	if(pDlg->m_isFoundHead)
	{
		memcpy(&pDlg->m_dstData[pDlg->m_nLen], pData, nDataLen);

		pDlg->m_nLen += nDataLen;
	}
	else if(pDlg->m_nLen < TEST_DATA_SIZE)
	{
		int i = 0;

		for(i = 0; i < nDataLen; i++)
		{
			if(0xA5 == pData[i])
			{
				pDlg->m_isFoundHead = TRUE;
				break;
			}	
		}

		if(pDlg->m_isFoundHead)
		{
			pDlg->m_nLen = nDataLen - i;
			memcpy(pDlg->m_dstData, &pData[i], pDlg->m_nLen);
		}
	}

	if(pDlg->m_nLen >= TEST_DATA_SIZE)
	{
		if(memcmp(g_TestDataDst, pDlg->m_dstData, TEST_DATA_SIZE) != 0)
		{
			pDlg->m_nErrCounter++;
		}
		else
		{
			pDlg->m_pLog->Log("\nUart test: PASSED.\n");
			pDlg->m_TestResultCtrl.ShowWindow(TRUE);
			pDlg->m_TestResultCtrl.SetScript(L"<P align=center><font face=verdana size=7 color=\"white\"><b>PASSED</b></font></p>");
			pDlg->m_TestResultCtrl.SetBackgroundColour(RGB(0,255,0));
			pDlg->TestReset();
			pDlg->UpdateProcessCtrl(DOWNLOAD_END, 0);
		}
	}
}

void CDlgUartTester::UpdateProcessCtrl(DWORD msg, UINT pos)
{
	::AfxGetMainWnd()->PostMessage(WM_DOWNLOAD, msg, pos);

	if(DOWNLOAD_END == msg)
	{
		CMainDlg* pDlg = (CMainDlg*)AfxGetMainWnd();
		pDlg->RegsterRcvDataCb(NULL, NULL);
	}
}

 BOOL CDlgUartTester::PreTranslateMessage(MSG* pMsg)
 {
	 if(pMsg->message==WM_KEYDOWN   &&   pMsg->wParam==0)
	 {
	 }
	 return CTabDlg::PreTranslateMessage(pMsg); 
 }

void CDlgUartTester::OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam)
{
	if(EVENT_USB_PLUG == eEvent)
	{
		GetDlgItem(IDC_UART_TEST_START)->EnableWindow(param);
	}
}


void CDlgUartTester::OnEnChangeResendCount()
{
	m_ReSendCountCtrl.Save();
	m_nMaxReSendCount = m_ReSendCountCtrl.GetValue();
}
