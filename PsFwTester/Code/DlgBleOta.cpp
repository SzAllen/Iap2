// DlgBleOta.cpp : 实现文件
//

#include "stdafx.h"
#include "Tester.h"
#include "DlgBleOta.h"
#include "afxdialogex.h"

// CDlgBleOta 对话框

IMPLEMENT_DYNAMIC(CDlgBleOta, CTabDlg)

CDlgBleOta::CDlgBleOta(CLog *pLog, CWnd* pParent /*=NULL*/)
	: CTabDlg(pLog, CDlgBleOta::IDD, pParent)
{

}

CDlgBleOta::~CDlgBleOta()
{
	CFileFind find;
	if(find.FindFile(m_strOtaFullFileName))
	{
		CFile file;
		file.Remove(m_strOtaFullFileName);
	}
}

void CDlgBleOta::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_BIN_FILE	, m_BinFileName);
	DDX_Control(pDX, IDC_EDIT_MTP		, m_OtaFilePath);

	DDX_Control(pDX,IDC_EDIT_MAIN_VERSION	, m_MainVerCtrl);
	DDX_Control(pDX,IDC_EDIT_MINOR_VER		, m_MinorVerCtrl);
	DDX_Control(pDX,IDC_EDIT_REV			, m_RevisionNumCtrl);
	DDX_Control(pDX,IDC_EDIT_BUILDNUM		, m_BuildNumCtrl);

	DDX_Text(pDX, IDC_EDIT_MAIN_VERSION	, m_nMainVer);
	DDX_Text(pDX, IDC_EDIT_MINOR_VER	, m_MinorVer);
	DDX_Text(pDX, IDC_EDIT_REV			, m_RevisionNum);
	DDX_Text(pDX, IDC_EDIT_BUILDNUM		, m_BuildNum);

	DDV_MinMaxByte (pDX, m_nMainVer		, 0, 255);
	DDV_MinMaxByte (pDX, m_MinorVer		, 0, 255);
	DDV_MinMaxByte (pDX, m_RevisionNum	, 0, 255);
	DDV_MinMaxDWord(pDX, m_BuildNum		, 0, 0xFFFFFFFF);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD, m_BtnDownLoad);
}


BEGIN_MESSAGE_MAP(CDlgBleOta, CTabDlg)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE, &CDlgBleOta::OnBnClickedButtonGenerate)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CDlgBleOta::OnBnClickedButtonDownload)
	ON_BN_CLICKED(IDC_BTN_BIN, &CDlgBleOta::OnBnClickedBtnBin)
	ON_BN_CLICKED(IDC_BTN_MTP, &CDlgBleOta::OnBnClickedBtnMtp)
END_MESSAGE_MAP()

BOOL CDlgBleOta::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	m_BinFileName .Init(&g_Profile, L"Setting", L"BleBinFileName" , L"");
	m_OtaFilePath .Init(&g_Profile, L"Setting", L"BleOtaFilePath" , L".\\");

	m_MainVerCtrl	 .Init(&g_Profile, L"Setting", L"BleMainVer"	, L"");
	m_MinorVerCtrl	 .Init(&g_Profile, L"Setting", L"BleMinorVer"	, L"");
	m_RevisionNumCtrl.Init(&g_Profile, L"Setting", L"BleRevNum"		, L"");
	m_BuildNumCtrl	 .Init(&g_Profile, L"Setting", L"BleBuildNum"	, L"");


	UpdateUI();

	return TRUE;
}
// CDlgBleOta 消息处理程序


void CDlgBleOta::OnBnClickedButtonGenerate()
{
	if(!UpdateData())
	{
		return;
	}

	m_MainVerCtrl	 .Save();
	m_MinorVerCtrl	 .Save();
	m_RevisionNumCtrl.Save();
	m_BuildNumCtrl	 .Save();

	COtaFile otaFile;

	if(!otaFile.Open(m_BinFileName.GetString(), CFile::modeRead))
	{
		LOG(L"Open file[%s] failed.\n", otaFile.GetFileName());
		return;
	}

	if(otaFile.IsCrcedFile())
	{
		LOG(L"File[%s] is OTA format.\n", otaFile.GetFileName());
		return;
	}

	CString strOtaFileName;
	strOtaFileName.Format(L"BleFwForOta_%d.%d.%d.%d.bin"
		, m_MainVerCtrl.GetValue()
		, m_MinorVerCtrl.GetValue()
		, m_RevisionNumCtrl.GetValue()
		, m_BuildNumCtrl.GetValue()
		);

	CString strOtaFullFileName;
	strOtaFullFileName.Format(L"%s\\%s", m_OtaFilePath.GetString(), strOtaFileName);

	int iRet = otaFile.ConvertBleCrcedFile(m_MainVerCtrl.GetValue()
		, m_MinorVerCtrl.GetValue()
		, m_RevisionNumCtrl.GetValue()
		, m_BuildNumCtrl.GetValue()
		, strOtaFullFileName
		);

	LOG(L"Convert to Ota format[ %s ] %s.\n", strOtaFileName, iRet ? L"SUCCESS" : L"FAILED");
}


void CDlgBleOta::OnBnClickedButtonDownload()
{
	BOOL isTemp = FALSE;
	m_strOtaFullFileName = m_BinFileName.GetString();
	COtaFile otaFile;

	if(!UpdateData())
	{
		return;
	}

	m_MainVerCtrl	 .Save();
	m_MinorVerCtrl	 .Save();
	m_RevisionNumCtrl.Save();
	m_BuildNumCtrl	 .Save();

	if(!otaFile.Open(m_BinFileName.GetString(), CFile::modeRead))
	{
		LOG(L"Open file[%s] failed.\n", m_BinFileName.GetString());
		return;
	}

	if(!otaFile.IsCrcedFile())
	{
		CString strPath = otaFile.GetFilePath();
		CString strFileName = otaFile.GetFileName();
		strPath.Replace(strFileName,L'\0');
		m_strOtaFullFileName.Format(L"%s$BleTemp.bin", strPath);

		int iRet = otaFile.ConvertBleCrcedFile(m_MainVerCtrl.GetValue()
			, m_MinorVerCtrl.GetValue()
			, m_RevisionNumCtrl.GetValue()
			, m_BuildNumCtrl.GetValue()
			, m_strOtaFullFileName
			);
		if(iRet == 0)
		{
			LOG(L"Convert file to OTA format failed.\n");
			return ;
		}

		otaFile.Close();

		isTemp = true;
		if(!otaFile.Open(m_strOtaFullFileName, CFile::modeRead))
		{
			LOG(L"Open file[%s] failed.\n", m_strOtaFullFileName);
			return;
		}
	}
	otaFile.Close();

	m_pDisk->StartDownLoadProcess(m_strOtaFullFileName, CUDisk::BLE_OTA_FILE);
	
	if(!isTemp)
	{
		m_strOtaFullFileName = L"";
	}
}


void CDlgBleOta::OnBnClickedBtnBin()
{
	m_BinFileName.OpenFileDlg(_T("Binary File (*.bin)|*.bin|"));

	UpdateUI();
}


void CDlgBleOta::OnBnClickedBtnMtp()
{
	m_OtaFilePath.OpenFolderDlg();
}

void CDlgBleOta::OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam)
{
	if(eEvent == EVENT_USB_PLUG)
	{
		GetDlgItem(IDC_BUTTON_DOWNLOAD)->EnableWindow(param);
	}
	else if(eEvent == EVENT_DOWNLOAD_PROCESS)
	{
		if(param == DOWNLOAD_START)
		{
			m_BtnDownLoad.EnableWindow(FALSE);
		}
		else if(param == DOWNLOAD_END)
		{
			CFileFind find;
			CFile file;
			if(find.FindFile(m_strOtaFullFileName) && file.Open(m_strOtaFullFileName, CFile::readOnly))
			{
				file.Close();
				file.Remove(m_strOtaFullFileName);
			}
			m_BtnDownLoad.EnableWindow(TRUE);
		}
	}
}

void CDlgBleOta::UpdateUI()
{
	COtaFile		otfFile;
	if(!otfFile.Open(m_BinFileName.GetString(), CFile::modeRead))
	{
		//LOG(L"Open file[%s] failed.\n", m_BinFileName.GetString());
	}
	else
	{
		//LOG(L"File[%s] is %s format.\n", otfFile.GetFileName(), otfFile.IsCrcedFile() ? L"OTA":L"RAW");
	}

	BOOL isOtaFormat = FALSE;
	if(otfFile.isOpen() && otfFile.IsCrcedFile())
	{
		isOtaFormat = TRUE;
		CrcSector* pSector = &otfFile.m_CrcSector;

		m_MainVerCtrl	 .SetValue(pSector->m_MainVer);
		m_MinorVerCtrl	 .SetValue(pSector->m_MinorVer);
		m_RevisionNumCtrl.SetValue(pSector->m_RevisionNum);
		m_BuildNumCtrl	 .SetValue(pSector->m_BuildNum);
	}

	GetDlgItem(IDC_EDIT_MAIN_VERSION)->EnableWindow(!isOtaFormat);
	GetDlgItem(IDC_EDIT_MINOR_VER)	->EnableWindow(!isOtaFormat);
	GetDlgItem(IDC_EDIT_REV)		->EnableWindow(!isOtaFormat);
	GetDlgItem(IDC_EDIT_BUILDNUM)	->EnableWindow(!isOtaFormat);
	GetDlgItem(IDC_BTN_MTP)			->EnableWindow(!isOtaFormat);
	GetDlgItem(IDC_BUTTON_GENERATE)	->EnableWindow(!isOtaFormat);
	GetDlgItem(IDC_EDIT_MTP)		->EnableWindow(!isOtaFormat);

	//GetDlgItem(IDC_BUTTON_DOWNLOAD)	->EnableWindow(isOtaFormat);

}
