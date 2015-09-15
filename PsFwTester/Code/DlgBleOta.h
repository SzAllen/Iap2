#pragma once

#include "XTabCtrl.h"
#include "editfilename.h"
#include "afxwin.h"
#include "OtaFile.h"
// CDlgBleOta 对话框


class CDlgBleOta : public CTabDlg
{
	DECLARE_DYNAMIC(CDlgBleOta)

public:
	CDlgBleOta(CLog *pLog, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgBleOta();

// 对话框数据
	enum { IDD = IDD_DLG_GENERATE_BLE_OTA };

protected:
	void UpdateUI();

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam);

	DECLARE_MESSAGE_MAP()

protected:
	CString			m_strOtaFullFileName;
	BOOL			m_isOtaFormat;
	CButton			m_BtnDownLoad;

	CEditFileName	m_BinFileName;
	CEditFileName	m_OtaFilePath;

	CEditNum	m_MainVerCtrl	;
	CEditNum	m_MinorVerCtrl	;
	CEditNum	m_RevisionNumCtrl;
	CEditNum	m_BuildNumCtrl	;

	BYTE  m_nMainVer;
	BYTE  m_MinorVer;
	BYTE  m_RevisionNum;
	DWORD m_BuildNum;
public:
	afx_msg void OnBnClickedButtonGenerate();
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedBtnBin();
	afx_msg void OnBnClickedBtnMtp();
};
