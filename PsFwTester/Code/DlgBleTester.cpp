// DlgDownLoad.cpp : 实现文件
//

#include "stdafx.h"
#include "Tester.h"
#include "DlgBleTester.h"
#include "afxdialogex.h"

#define REQ_SET_BURN_IN_STATE		0x1E
#define REQ_GET_BURN_IN_STATE		0x1F
#define OPERATIONAL_AMPLIFIERS_CALIBRATION		0x32

// CDlgBleTester 对话框

IMPLEMENT_DYNAMIC(CDlgBleTester, CTabDlg)

CDlgBleTester::CDlgBleTester(CLog *pLog, CWnd* pParent /*=NULL*/)
	: CTabDlg(pLog, CDlgBleTester::IDD, pParent)
{

}

CDlgBleTester::~CDlgBleTester()
{
}

void CDlgBleTester::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(CDlgBleTester, CTabDlg)
	ON_BN_CLICKED(IDC_BTN_SET_BURN_IN, &CDlgBleTester::OnBnClickedSetBurnInFlag)
	ON_BN_CLICKED(IDC_BTN_CLEAN_BURN_IN, &CDlgBleTester::OnBnClickedCleanBurnInFlag)
	ON_BN_CLICKED(IDC_BTN_OAC, &CDlgBleTester::OnBnClickedBtnOac)
END_MESSAGE_MAP()


BOOL CDlgBleTester::OnInitDialog()
{
	CTabDlg::OnInitDialog();


	return TRUE;
}

int CDlgBleTester::SendBlePassThroughCmd(LPCSTR title, BYTE cmd, const BYTE* pData, BYTE nDatalen, BYTE* pBuff, BYTE nBuffLen)
{
	BYTE copyLen = 0;
	BOOL bRet = FALSE;
	BYTE inData[SECTOR_SIZE] = {0};
	DWORD nInDataLen = nBuffLen == 0 ? 1 : nBuffLen;
	BYTE cbd[16] = {0};

	if(nBuffLen == 0)
	{
		nBuffLen = 1;
	}

	cbd[0] = CUSTOM_BLE_PASS_THROUGH;

	cbd[1] = cmd;
	cbd[2] = nDatalen;
	memcpy(&cbd[3], pData, nDatalen);

	bRet = m_pDisk->DataIn(cbd, sizeof(cbd), inData, nInDataLen);

	if(bRet)
	{
		bRet = (inData[0] == 0);
	}

	LOG("Send %s CMD %s.\n", title, bRet ? "Success" : "Failed");
	//m_pLog->DumpByte(&cbd[1], nDatalen + 2);

	m_pLog->DumpByte(inData, (short)nInDataLen);
	if(!bRet)
	{
		LOG("Dump Rsp Data: ");
	}
	else if(pBuff && nBuffLen)
	{
		copyLen = (BYTE)(nBuffLen > nInDataLen ? nInDataLen : nBuffLen);
		memcpy(pBuff, inData, nBuffLen);
	}

	return copyLen;
}

void CDlgBleTester::OnBnClickedSetBurnInFlag()
{
	BYTE cmdData[] = {1};
	SendBlePassThroughCmd("SET_BURN_IN_STATE", REQ_SET_BURN_IN_STATE, cmdData, sizeof(cmdData));
}

void CDlgBleTester::OnBnClickedCleanBurnInFlag()
{
	BYTE cmdData[] = {0};
	SendBlePassThroughCmd("CLEAN_BURN_IN_STATE", REQ_SET_BURN_IN_STATE, cmdData, sizeof(cmdData));
}


void CDlgBleTester::OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam)
{
	if(eEvent == EVENT_USB_PLUG)
	{
		GetDlgItem(IDC_BTN_SET_BURN_IN)	 ->EnableWindow(param);
		GetDlgItem(IDC_BTN_CLEAN_BURN_IN)->EnableWindow(param);
		GetDlgItem(IDC_BTN_OAC)->EnableWindow(param);
	}
	else if(eEvent == EVENT_DOWNLOAD_PROCESS)
	{
		if(param == DOWNLOAD_START)
		{
			//m_BtnDownLoad.EnableWindow(FALSE);
		}
		else if(param == DOWNLOAD_END)
		{
			//m_BtnDownLoad.EnableWindow(TRUE);
		}
	}
}




void CDlgBleTester::OnBnClickedBtnOac()
{
	BYTE cmdData[] = {0};
	SendBlePassThroughCmd("OPERATIONAL_AMPLIFIERS_CALIBRATION", OPERATIONAL_AMPLIFIERS_CALIBRATION, cmdData, sizeof(cmdData), Null, 3);
}
