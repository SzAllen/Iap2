
// MainDlg.h : 头文件
//

#pragma once

#include "XTabCtrl.h"
#include "afxcmn.h"
#include "LogCtrl.h"
#include "resource.h"
#include "SerialMFC.h"
#include "afxwin.h"
#include "EditFileName.h"
#include "HTMLStatic.h"

#define WM_LOG			(WM_USER + 1)
#define WM_DOWNLOAD		(WM_USER + 2)
#define WM_USB_PULG_IN	(WM_USER + 3)
#define WM_USB_PULG_OUT	(WM_USER + 4)
#define WM_DLG_INIT		(WM_USER + 5)
#define WM_UTP_RUN		(WM_USER + 6)
#define WM_TX_DATA		(WM_USER + 7)

#define DOWNLOAD_START		0
#define DOWNLOAD_END		1
#define DOWNLOAD_POS		2

#define UART_FLAG_TX		0
#define UART_FLAG_RX		1

typedef void (*HandlerFn)(CTabDlg* pDlg, const void* pData, size_t iLen);
// CMainDlg 对话框
class CMainDlg : public CDialogEx, CLog
{
// 构造
public:
	CMainDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CSerialMFC	m_Serial;
protected:
	CStringArray	m_ComPortList;
	CTabDlg*		m_pHanderArg;
	HandlerFn		Hander;

	BYTE			m_TxDataFlag;
	CProgressCtrl	m_ProgCtrl;
	CXTabCtrl		m_tabctrl;
	CImageList		m_imagelistTab;
	CLogCtrl		m_LogCtrl;
	CComboProfile	m_ComListCtrl;
	CCheckBox		m_ChkShowTx;
	CCheckBox		m_ChkShowRx;
	CHTMLStatic		m_ComCfgLog;


	CString		m_strComPort;

//	CUDisk		m_Disk;
// 实现
protected:
	HICON m_hIcon;
	UINT_PTR m_TimerId;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();

	LRESULT OnMyDeviceChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLog(WPARAM wparam, LPARAM lp);
	afx_msg LRESULT OnDownload(WPARAM wparam, LPARAM lp);
	afx_msg LRESULT OnDlgInit(WPARAM wparam = 0, LPARAM lp = 0);
	afx_msg void OnBnClickedBtnClean();
	afx_msg void OnBnClickedBtnSetting();
	afx_msg void OnCbnSelchangeComNum();
	afx_msg LRESULT OnSerialMsg (WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedChkShowTx();
	afx_msg void OnBnClickedChkShowRx();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT nIDEvent);
public:
	afx_msg LRESULT OnTxData(const void* pData, UINT dataLen);

	DECLARE_MESSAGE_MAP()
public:
	BOOL RegsterRcvDataCb(CTabDlg* pDlg, HandlerFn rcvHandler);
	virtual void Reset();
	virtual void Log(LPCTSTR pszFormat, ...);
	virtual void Log(const char* pszFormat, ...);
	virtual void Notify(EVENT eEvent, WPARAM param = 0, LPARAM lparam = 0);
	void UpdateComUI();
	void OpenCom();
	void DisplayEvent (LPCTSTR lpszMsg);
	void DisplayEventSetting (LPCTSTR lpszMsg, LPCTSTR lpszSetting, bool fOn);
	void OnDeviceChange();
};

