#pragma once


// CDlgDownLoad �Ի���
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
	CDlgDownLoad(CLog *pLog, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDownLoad();

// �Ի�������
	enum { IDD = IDD_DLG_USBFW_DOWNLOAD };

protected:
	BOOL VerifyOtaFile(LPCTSTR lpFileName, CrcSector& otaFileInfo);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
