#pragma once

#include "XTabCtrl.h"
#include "resource.h"
#include "afxwin.h"
#include "HTMLStatic.h"
#include "EditFileName.h"
// CDlgUartTester �Ի���

#define WM_RESEND		(WM_USER + 1)
#define TEST_DATA_SIZE	8
class CDlgUartTester : public CTabDlg
{
	DECLARE_DYNAMIC(CDlgUartTester)
public:
	CDlgUartTester(CLog *pLog, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgUartTester();

// �Ի�������
	enum { IDD = IDD_DLG_UART_TESTER };

protected:
	CHTMLStatic m_TestResultCtrl;
	CEditNum m_ReSendCountCtrl;
	BYTE m_nMaxReSendCount;

	int m_nReTxCounter;
	int m_nErrCounter;
	UINT_PTR m_TimerId;

	int		m_nLen;
	BYTE	m_dstData[TEST_DATA_SIZE];
	BOOL	m_isFoundHead;

protected:
	void UpdateProcessCtrl(DWORD msg, UINT pos = 0);
	void UartTestStart();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnNotify(EVENT eEvent, WPARAM param = 0, LPARAM lparam = 0);


	void TestReset();
	static void RxDataHandler(CDlgUartTester* pDlg, const uint8* pData, int nDataLen);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUartTestStart();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnReSend(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnChangeResendCount();
};
