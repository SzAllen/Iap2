#pragma once

#include "XTabCtrl.h"
#include "editfilename.h"
#include "afxwin.h"
//#include "OtaFile.h"
// CDlgOta 对话框

#define REQ_OTA_START  		0x16
#define REQ_OTA_UPDATE  	0x17
#define REQ_OTA_COMPLETE 	0x18
#define REQ_OTA_VERIFY  	0x19

#define WM_OTA_SEND			(WM_USER + 1)

class CDlgOta : public CTabDlg
{
	DECLARE_DYNAMIC(CDlgOta)
public:
	CDlgOta(CLog *pLog, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgOta();

// 对话框数据
	enum { IDD = IDD_DLG_GENERATE_OTA };

protected:
	void UpdateUI();

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam);
	virtual void SelChanged(CTabDlg* pNewTabDlg);

	DECLARE_MESSAGE_MAP()

public:
	BOOL DownloadOtaFile(CFile& file, DWORD offset);
	void DownloadDone();
	void UpdateProcessCtrl(DWORD msg, UINT pos = 0);
	CFile			m_Otafile;
	DWORD			m_dwOtaFileLen;
	BOOL			m_isUpdate;
	CTime			m_StartTime;
protected:
	int				m_TimerID;
	CString			m_strOtaFullFileName;
	BOOL			m_isOtaFormat;
	CButton			m_BtnDownLoad;

	CEditFileName	m_BinFileName;
	CEditFileName	m_OtaFilePath;

	CEditNum	m_MainVerCtrl	;
	CEditNum	m_MinorVerCtrl	;
	CEditNum	m_RevisionNumCtrl;
	CEditNum	m_BuildNumCtrl	;
	CEditProfile m_OtaFileNameCtrl	;

	BYTE  m_nMainVer;
	BYTE  m_MinorVer;
	BYTE  m_RevisionNum;
	DWORD m_BuildNum;
public:
	afx_msg void OnBnClickedButtonGenerate();
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedBtnBin();
	afx_msg void OnBnClickedBtnMtp();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OtaSend(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT UtpRun(WPARAM wParam, LPARAM lParam);
};

