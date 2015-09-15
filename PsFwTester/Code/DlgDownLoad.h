#pragma once


// CDlgDownLoad 对话框
#include "XTabCtrl.h"
#include "EditFileName.h"

class CDlgDownLoad : public CTabDlg
{
	DECLARE_DYNAMIC(CDlgDownLoad)

public:
	CrcSector	m_UsbOtaFileInfo;
	CrcSector	m_BleOtaFileInfo;

	CEditFileName	m_UsbOtaFileName;
	CEditFileName	m_BleOtaFileName;
	CButton			m_BtnDownLoad;
	CButton			m_ChkAuto;
public:
	CDlgDownLoad(CLog *pLog, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDownLoad();

// 对话框数据
	enum { IDD = IDD_DLG_USBFW_DOWNLOAD };

protected:
	BOOL VerifyOtaFile(LPCTSTR lpFileName, CrcSector& otaFileInfo);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnUsbOtaFile();
	afx_msg void OnBnClickedButtonDownloadUsbFw();
	afx_msg void OnBnClickedButtonDownloadBleFw();
	afx_msg void OnBnClickedButtonDownloadAll();
	afx_msg void OnBnClickedBtnBleOtaFile();
	afx_msg void OnBnClickedCheckAutoDlUsbFw();
};
