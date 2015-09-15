// DlgGenbootBinFile.cpp : 实现文件
//

#include "stdafx.h"
#include "Tester.h"
#include "DlgMtp.h"
#include "afxdialogex.h"
//#include "OtaFile.h"
#include "ElfFile.h"
#include "ImgFile.h"

// CDlgMtp 对话框

IMPLEMENT_DYNAMIC(CDlgMtp, CTabDlg)

CDlgMtp::CDlgMtp(CLog *pLog, CWnd* pParent /*=NULL*/)
	: CTabDlg(pLog, CDlgMtp::IDD, pParent)
	, m_nMainVer(0)
{

}

CDlgMtp::~CDlgMtp()
{
}

void CDlgMtp::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BL_FILE	, m_BootFileName);
	DDX_Control(pDX, IDC_EDIT_BIN_FILE	, m_BinFileName);
	DDX_Control(pDX, IDC_EDIT_MTP		, m_MtpFileName);

	DDX_Control(pDX,IDC_EDIT_MAIN_VERSION	, m_MainVerCtrl);
	DDX_Control(pDX,IDC_EDIT_MINOR_VER		, m_MinorVerCtrl);
	DDX_Control(pDX,IDC_EDIT_REV			, m_RevisionNumCtrl);
	DDX_Control(pDX,IDC_EDIT_BUILDNUM		, m_BuildNumCtrl);
	DDX_Control(pDX,IDC_EDIT_MTP_FILENAME	, m_MtpFileNameCtrl);

	DDX_Control(pDX,IDC_EDIT_HW_MAIN_VERSION, m_HwMainVerCtrl);
	DDX_Control(pDX,IDC_EDIT_HW_MINOR_VER	, m_HwMinorVerCtrl);
	DDX_Control(pDX,IDC_EDIT_HW_SUB_REV		, m_HwSubVerCtrl);

	DDX_Text(pDX, IDC_EDIT_MAIN_VERSION	, m_nMainVer);
	DDX_Text(pDX, IDC_EDIT_MINOR_VER	, m_MinorVer);
	DDX_Text(pDX, IDC_EDIT_REV			, m_RevisionNum);
	DDX_Text(pDX, IDC_EDIT_BUILDNUM		, m_BuildNum);

	DDX_Text(pDX, IDC_EDIT_HW_MAIN_VERSION	, m_HwMainVer);
	DDX_Text(pDX, IDC_EDIT_HW_MINOR_VER		, m_HwMinorVer);
	DDX_Text(pDX, IDC_EDIT_HW_SUB_REV		, m_HwSumNum);

	DDV_MinMaxByte (pDX, m_nMainVer		, 0, 255);
	DDV_MinMaxByte (pDX, m_MinorVer		, 0, 255);
	DDV_MinMaxByte (pDX, m_RevisionNum	, 0, 255);
	DDV_MinMaxDWord(pDX, m_BuildNum		, 0, 0xFFFFFFFF);
}


BEGIN_MESSAGE_MAP(CDlgMtp, CTabDlg)
	ON_BN_CLICKED(IDC_BTN_BL_FILE, &CDlgMtp::OnBnClickedBtnBlFile)
	ON_BN_CLICKED(IDC_BTN_BIN, &CDlgMtp::OnBnClickedBtnBin)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE, &CDlgMtp::OnBnClickedButtonGenerate)
	ON_BN_CLICKED(IDC_BTN_MTP, &CDlgMtp::OnBnClickedBtnMtp)
END_MESSAGE_MAP()


// CDlgMtp 消息处理程序
BOOL CDlgMtp::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	m_BootFileName.Init(&g_Profile, L"FilePath", L"BootFileName", L"", L"*Boot.img");
	m_BinFileName .Init(&g_Profile, L"FilePath", L"BinFileName" , L"");
	m_MtpFileName .Init(&g_Profile, L"FilePath", L"MtpFilePath" , L".\\");

	m_MainVerCtrl	 .Init(&g_Profile, L"Setting", L"MainVer"	, L"");
	m_MinorVerCtrl	 .Init(&g_Profile, L"Setting", L"MinorVer"	, L"");
	m_RevisionNumCtrl.Init(&g_Profile, L"Setting", L"RevNum"	, L"");
	m_BuildNumCtrl	 .Init(&g_Profile, L"Setting", L"BuildNum"	, L"");

	m_HwMainVerCtrl	 .Init(&g_Profile, L"Setting", L"HwMainVer"	, L"");
	m_HwMinorVerCtrl .Init(&g_Profile, L"Setting", L"HwMinorVer", L"");
	m_HwSubVerCtrl	 .Init(&g_Profile, L"Setting", L"HwSubNum"	, L"");

	m_MtpFileNameCtrl.Init(&g_Profile, L"FilePath", L"MtpFileName", L"UsbFwForJlink");

	return TRUE;
}

void CDlgMtp::SelChanged(CTabDlg* pNewTabDlg)
{
	m_BinFileName	 .Reflash();
	m_MtpFileName	 .Reflash();

	m_MainVerCtrl	 .Reflash();
	m_MinorVerCtrl	 .Reflash();
	m_RevisionNumCtrl.Reflash();
	m_BuildNumCtrl	 .Reflash();

	m_HwMainVerCtrl	 .Reflash();
	m_HwMinorVerCtrl .Reflash();
	m_HwSubVerCtrl	 .Reflash();
}

void CDlgMtp::OnBnClickedBtnBlFile()
{
	// TODO: 在此添加控件通知处理程序代码
	m_BootFileName.OpenFileDlg(_T("Binary File (*.elf)|*.elf|"));
}


void CDlgMtp::OnBnClickedBtnBin()
{
	// TODO: 在此添加控件通知处理程序代码
	m_BinFileName.OpenFileDlg(_T("Binary File (*.elf)|*.elf|"));
}

void CDlgMtp::OnBnClickedBtnMtp()
{
	m_MtpFileName.OpenFolderDlg();
}

void CDlgMtp::OnBnClickedButtonGenerate()
{

}

