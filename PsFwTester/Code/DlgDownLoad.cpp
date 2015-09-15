// DlgDownLoad.cpp : 实现文件
//

#include "stdafx.h"
#include "Tester.h"
#include "DlgDownLoad.h"
#include "afxdialogex.h"


// CDlgDownLoad 对话框

IMPLEMENT_DYNAMIC(CDlgDownLoad, CTabDlg)

CDlgDownLoad::CDlgDownLoad(CLog *pLog, CWnd* pParent /*=NULL*/)
	: CTabDlg(pLog, CDlgDownLoad::IDD, pParent)
{
	memset(&m_UsbOtaFileInfo, 0,sizeof(CrcSector));
	memset(&m_BleOtaFileInfo, 0,sizeof(CrcSector));
}

CDlgDownLoad::~CDlgDownLoad()
{
}

void CDlgDownLoad::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD		, m_BtnDownLoad); 
	DDX_Control(pDX, IDC_EDIT_USB_OTA_FILENAME	, m_UsbOtaFileName);
	DDX_Control(pDX, IDC_EDIT_BLE_OTA_FILENAME	, m_BleOtaFileName);
	DDX_Control(pDX, IDC_CHECK_AUTO_DL_USB		, m_ChkAuto);
}


BEGIN_MESSAGE_MAP(CDlgDownLoad, CTabDlg)
	ON_BN_CLICKED(IDC_BTN_USB_OTA_FILE		, &CDlgDownLoad::OnBnClickedBtnUsbOtaFile)
	ON_BN_CLICKED(IDC_BTN_BLE_OTA_FILE		, &CDlgDownLoad::OnBnClickedBtnBleOtaFile)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_USB_FW, &CDlgDownLoad::OnBnClickedButtonDownloadUsbFw)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_BLE_FW, &CDlgDownLoad::OnBnClickedButtonDownloadBleFw)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD		, &CDlgDownLoad::OnBnClickedButtonDownloadAll)
	ON_BN_CLICKED(IDC_CHECK_AUTO_DL_USB, &CDlgDownLoad::OnBnClickedCheckAutoDlUsbFw)
END_MESSAGE_MAP()


BOOL CDlgDownLoad::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	m_ChkAuto.SetCheck(FALSE);

	m_UsbOtaFileName.Init(&g_Profile, L"Setting", L"UsbOtaFileName", L"", L"*UsbFwForOta_*.bin");
	m_BleOtaFileName.Init(&g_Profile, L"Setting", L"BleOtaFileName", L"", L"*BleFwForOta*.bin");

	COtaFile::GetOtaFileInfo(m_UsbOtaFileName.GetString(), m_UsbOtaFileInfo);
	COtaFile::GetOtaFileInfo(m_BleOtaFileName.GetString(), m_BleOtaFileInfo);

	return TRUE;
}

void CDlgDownLoad::OnBnClickedBtnUsbOtaFile()
{
	m_UsbOtaFileName.OpenFileDlg(_T("Binary File (*.bin)|*.bin|"));
	COtaFile::GetOtaFileInfo(m_UsbOtaFileName.GetString(), m_UsbOtaFileInfo);

	OnBnClickedCheckAutoDlUsbFw();
}

void CDlgDownLoad::OnBnClickedBtnBleOtaFile()
{
	m_BleOtaFileName.OpenFileDlg(_T("Binary File (*.bin)|*.bin|"));
	COtaFile::GetOtaFileInfo(m_BleOtaFileName.GetString(), m_BleOtaFileInfo);
	OnBnClickedCheckAutoDlUsbFw();
}

void CDlgDownLoad::OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam)
{
	if(eEvent == EVENT_USB_PLUG)
	{
		GetDlgItem(IDC_BUTTON_DOWNLOAD)			->EnableWindow(param);
		GetDlgItem(IDC_BUTTON_DOWNLOAD_USB_FW)	->EnableWindow(param);
		GetDlgItem(IDC_BUTTON_DOWNLOAD_BLE_FW)	->EnableWindow(param);
		if(param)
		{
			if(m_ChkAuto.GetCheck())
			{
				LOG(L"======================================\n");
				if(m_pDisk->m_pInquiryInfo->m_UsbBuildNum != m_UsbOtaFileInfo.m_BuildNum)
				{
					OnBnClickedButtonDownloadUsbFw();
				}
				else 
				{
					LOG(L"USB Firmware Upgrade SUCCESS.\n");
					if(m_pDisk->m_pInquiryInfo->m_BleBuildNum != m_BleOtaFileInfo.m_BuildNum)
					{
						OnBnClickedButtonDownloadBleFw();
					}
					else
					{
						LOG(L"BLE Firmware Upgrade SUCCESS.\n");
					}
				}
				LOG(L"======================================\n");
			}
		}
		else
		{
			m_pLog->Reset();
		}
	}
	else if(eEvent == EVENT_DOWNLOAD_PROCESS)
	{
		if(param == DOWNLOAD_START)
		{
			m_BtnDownLoad.EnableWindow(FALSE);
		}
		else if(param == DOWNLOAD_END)
		{
			m_BtnDownLoad.EnableWindow(TRUE);

			if(lparam == 1)
			{
				if(m_pDisk->m_pInquiryInfo->m_BleBuildNum != m_BleOtaFileInfo.m_BuildNum)
				{
					OnBnClickedButtonDownloadBleFw();
				}
				else
				{
					LOG(L"BLE Firmware Upgrade SUCCESS.\n");
				}
			}
			else if(lparam == 2)
			{
			}
		}
	}
}

BOOL CDlgDownLoad::VerifyOtaFile(LPCTSTR lpFileName, CrcSector& otaFileInfo)
{
	COtaFile otaFile;
	if(!otaFile.Open(lpFileName, CFile::modeRead))
	{
		LOG(L"File path is invalid, open file[%s] failed.\n", otaFile.GetFileName());
		return FALSE;
	}

	if(!otaFile.IsCrcedFile())
	{
		LOG(L"File[%s] is not OTA format.\n", otaFile.GetFileName());
		return FALSE;
	}
	memcpy(&otaFileInfo, &otaFile.m_CrcSector, sizeof(CrcSector));
	otaFile.Close();

	return TRUE;
}

void CDlgDownLoad::OnBnClickedButtonDownloadUsbFw()
{
	BOOL bFlag = TRUE;

	memset(&m_UsbOtaFileInfo, 0,sizeof(CrcSector));
	if(VerifyOtaFile(m_UsbOtaFileName.GetString(), m_UsbOtaFileInfo))
	{
		if(m_ChkAuto.GetCheck())
		{
			if(m_pDisk->m_pInquiryInfo->m_BleBuildNum != m_BleOtaFileInfo.m_BuildNum)
			{
				bFlag = FALSE;
			}
		}
		m_pDisk->StartDownLoadProcess(m_UsbOtaFileName.GetString(), CUDisk::USB_OTA_FILE, bFlag);
	}
}

void CDlgDownLoad::OnBnClickedButtonDownloadBleFw()
{
	memset(&m_BleOtaFileInfo, 0,sizeof(CrcSector));
	if(VerifyOtaFile(m_BleOtaFileName.GetString(), m_BleOtaFileInfo))
	{
		m_pDisk->StartDownLoadProcess(m_BleOtaFileName.GetString(), CUDisk::BLE_OTA_FILE);
	}
}

void CDlgDownLoad::OnBnClickedButtonDownloadAll()
{
	if(m_pDisk->m_pInquiryInfo == NULL) return;

	if(m_pDisk->m_pInquiryInfo->m_UsbBuildNum != m_UsbOtaFileInfo.m_BuildNum)
	{
		memset(&m_UsbOtaFileInfo, 0,sizeof(CrcSector));
		if(VerifyOtaFile(m_UsbOtaFileName.GetString(), m_UsbOtaFileInfo))
		{
			m_pDisk->StartDownLoadProcess(m_UsbOtaFileName.GetString(), CUDisk::USB_OTA_FILE, FALSE);
		}
	}
	else if(m_pDisk->m_pInquiryInfo->m_BleBuildNum != m_BleOtaFileInfo.m_BuildNum)
	{
		memset(&m_BleOtaFileInfo, 0,sizeof(CrcSector));
		if(VerifyOtaFile(m_BleOtaFileName.GetString(), m_UsbOtaFileInfo))
		{
			m_pDisk->StartDownLoadProcess(m_BleOtaFileName.GetString(), CUDisk::BLE_OTA_FILE);
		}
	}
}


void CDlgDownLoad::OnBnClickedCheckAutoDlUsbFw()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_ChkAuto.GetCheck() && m_pDisk->IsValid())
	{
		OnNotify(EVENT_USB_PLUG, TRUE, 0);
	}
}
