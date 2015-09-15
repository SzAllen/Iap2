// DlgDownLoad.cpp : 实现文件
//

#include "stdafx.h"
#include "Tester.h"
#include "DlgMoudleNum.h"
#include "afxdialogex.h"


// CDlgMoudleNum 对话框

IMPLEMENT_DYNAMIC(CDlgMoudleNum, CTabDlg)

CDlgMoudleNum::CDlgMoudleNum(CLog *pLog, CWnd* pParent /*=NULL*/)
	: CTabDlg(pLog, CDlgMoudleNum::IDD, pParent)
{

}

CDlgMoudleNum::~CDlgMoudleNum()
{
}

void CDlgMoudleNum::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD, m_BtnDownLoad);
	DDX_Control(pDX, IDC_EDIT_LOG_FILE	, m_CtrlLogFile);

	DDX_Control(pDX, IDC_COMBO_COLOR, m_ComboColor);
	DDX_Control(pDX, IDC_COMBO_CAPA, m_ComboCapa);
	DDX_Control(pDX, IDC_COMBO_PRODUCT, m_ComboProduct);
}


BEGIN_MESSAGE_MAP(CDlgMoudleNum, CTabDlg)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CDlgMoudleNum::OnBnClickedButtonDownload)
	ON_BN_CLICKED(IDC_BTN_LOGFILE, &CDlgMoudleNum::OnBnClickedBtnLogfile)
END_MESSAGE_MAP()


BOOL CDlgMoudleNum::OnInitDialog()
{
	int i = 0;
	CTabDlg::OnInitDialog();

	m_CtrlLogFile.Init(&g_Profile, L"Setting", L"SetMoudleNumLogFileFolder", L"", L"*Log_*.txt");

	m_ComboColor.DeleteString(0);
//	m_ComboColor.AddString(L"WHITE");	m_ComboColor.SetItemData(i++, COLOR_WHITE);
	m_ComboColor.AddString(L"BLACK");	m_ComboColor.SetItemData(i++, COLOR_BLACK);
//	m_ComboColor.AddString(L"RED");		m_ComboColor.SetItemData(i++, COLOR_RED);
	m_ComboColor.AddString(L"GLODEN");	m_ComboColor.SetItemData(i++, COLOR_GLODEN);
	m_ComboColor.SetCurSel(0);
	m_ComboColor.EnableWindow(TRUE);

	m_ComboProduct.SetCurSel(0);

	i = 0;
	m_ComboCapa.AddString(L"0 G");	m_ComboCapa.SetItemData(i++, 0);
	m_ComboCapa.AddString(L"32 G");	m_ComboCapa.SetItemData(i++, CAPACITY_32G);
	m_ComboCapa.AddString(L"64 G");	m_ComboCapa.SetItemData(i++, CAPACITY_64G);
	m_ComboCapa.AddString(L"128 G");m_ComboCapa.SetItemData(i++, CAPACITY_128G);
	m_ComboCapa.SetCurSel(0);

	return TRUE;
}

void CDlgMoudleNum::OnBnClickedBtnLogfile()
{
	m_CtrlLogFile.OpenFolderDlg();
}

void CDlgMoudleNum::OnBnClickedButtonDownload()
{
	CString str;

	//if(m_CtrlLogFile.GetString() == L"")
	//{
	//	MessageBox(L"Log file path is error.", L"Error", MB_OK|MB_ICONSTOP);
	//	return;
	//}

	//m_UsbDisk.FormatLog(TRUE, "FLK-SerialNumber=%s.\n", str);
	m_ComboCapa.GetCurSel();
	BYTE capa = (BYTE)m_ComboCapa.GetItemData(m_ComboCapa.GetCurSel());
	BYTE color = (BYTE)m_ComboColor.GetItemData(m_ComboColor.GetCurSel());
	if(m_pDisk->WriteMoudleNumber(color, capa, m_ComboProduct.GetCurSel() == 1))
	{
		return;
	}

	BYTE cbd[] = {CUSTOM_SET_BURN_IN_STATE, 0};
	if(m_pDisk->DataNone(cbd, sizeof(cbd)))
	{
		m_pDisk->FormatLog("CUSTOM_SET_BURN_IN_STATE failed.\n");
		return;
	}

}
void CDlgMoudleNum::SetComboCapa(BYTE capaAllignG)
{
	if(capaAllignG == 32)
	{
		m_ComboCapa.SetCurSel(1);
	}
	else if(capaAllignG == 64)
	{
		m_ComboCapa.SetCurSel(2);
	}
	else if(capaAllignG == 128)
	{
		m_ComboCapa.SetCurSel(3);
	}
	else
	{
		m_ComboCapa.SetCurSel(0);
	}
}

void CDlgMoudleNum::OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam)
{
	if(eEvent == EVENT_USB_PLUG)
	{
		GetDlgItem(IDC_BUTTON_DOWNLOAD)->EnableWindow(param);
		SetComboCapa(m_pDisk->m_CapacityAllignG);
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
		}
	}
}


