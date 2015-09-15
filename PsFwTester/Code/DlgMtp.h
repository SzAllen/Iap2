#pragma once

#include "XTabCtrl.h"
#include "editfilename.h"

// CDlgMtp 对话框
class CDlgMtp : public CTabDlg
{
	DECLARE_DYNAMIC(CDlgMtp)

public:
	CDlgMtp(CLog *pLog, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMtp();

// 对话框数据
	enum { IDD = IDD_DLG_GENERATE_MTP };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void SelChanged(CTabDlg* pNewTabDlg);


	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnBlFile();
	afx_msg void OnBnClickedBtnBin();

protected:
	CEditFileName	m_BootFileName;
	CEditFileName	m_BinFileName;
	CEditFileName	m_MtpFileName;
	CEditProfile	m_MtpFileNameCtrl;

	CEditNum	m_MainVerCtrl	;
	CEditNum	m_MinorVerCtrl	;
	CEditNum	m_RevisionNumCtrl;
	CEditNum	m_BuildNumCtrl	;

	CEditNum	m_HwMainVerCtrl	;
	CEditNum	m_HwMinorVerCtrl	;
	CEditNum	m_HwSubVerCtrl;

	BYTE  m_nMainVer;
	BYTE  m_MinorVer;
	BYTE  m_RevisionNum;
	DWORD m_BuildNum;

	BYTE  m_HwMainVer;
	BYTE  m_HwMinorVer;
	BYTE  m_HwSumNum;

public:
	afx_msg void OnBnClickedButtonGenerate();
	afx_msg void OnBnClickedBtnMtp();
};
