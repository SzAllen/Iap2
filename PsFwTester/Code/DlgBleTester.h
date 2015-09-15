#pragma once


// CDlgBleTester 对话框
#include "XTabCtrl.h"
#include "EditFileName.h"

class CDlgBleTester : public CTabDlg
{
	DECLARE_DYNAMIC(CDlgBleTester)

public:
public:
	CDlgBleTester(CLog *pLog, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgBleTester();

// 对话框数据
	enum { IDD = IDD_DLG_BLE_TESTER };

protected:
	int SendBlePassThroughCmd(LPCSTR title, BYTE cmd, const BYTE* pData = NULL, BYTE nDatalen = 0, BYTE* pBuff = NULL, BYTE nBuffLen = 0);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSetBurnInFlag();
	afx_msg void OnBnClickedCleanBurnInFlag();
	afx_msg void OnBnClickedBtnOac();
};
