#pragma once


// CDlgBleTester �Ի���
#include "XTabCtrl.h"
#include "EditFileName.h"

class CDlgBleTester : public CTabDlg
{
	DECLARE_DYNAMIC(CDlgBleTester)

public:
public:
	CDlgBleTester(CLog *pLog, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgBleTester();

// �Ի�������
	enum { IDD = IDD_DLG_BLE_TESTER };

protected:
	int SendBlePassThroughCmd(LPCSTR title, BYTE cmd, const BYTE* pData = NULL, BYTE nDatalen = 0, BYTE* pBuff = NULL, BYTE nBuffLen = 0);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSetBurnInFlag();
	afx_msg void OnBnClickedCleanBurnInFlag();
	afx_msg void OnBnClickedBtnOac();
};
