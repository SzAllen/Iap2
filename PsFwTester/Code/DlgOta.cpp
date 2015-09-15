// DlgOta.cpp : 实现文件
//

#include "stdafx.h"
#include "MainApp.h"
#include "DlgOta.h"
#include "afxdialogex.h"
#include "OtaFile.h"
#include "MainDlg.h"
// CDlgOta 对话框

static UtpMgr* g_pUtpMgr;

static void Utp_RxDataHandler(CDlgOta* pDlg, const uint8* pData, int nDataLen)
{
	if(g_pUtpMgr)
	{
		UtpMgr_Parse(g_pUtpMgr, NULL, pData, nDataLen);
		UtpMgr_Run(g_pUtpMgr);
	}
}

int Utp_Tx(const uint8* pData, int nDataLen)
{
	CMainDlg* pDlg = (CMainDlg*)AfxGetMainWnd();
	pDlg->OnTxData(pData, nDataLen);
	return 0;
}

void Utp_RspProc(void* pObj, UtpPkt* pReqPkt, uint8* pRsp, uint8 rspLen, uint8 errCode)
{
	CDlgOta* pCDlgOta = (CDlgOta*)pObj;

	if(!pCDlgOta->m_isUpdate)
	{
		pCDlgOta->DownloadDone();
		pCDlgOta->m_pLog->Log("Update is stoped.\n");
		return;
	}

	if(errCode != RC_SUCCESS)
	{
		pCDlgOta->m_pLog->Log("Send Req[0x%X] Error, errCode=%d.\n", pReqPkt->m_Cmd, errCode);
		pCDlgOta->DownloadDone();
		return;
	}

	if(pRsp[0] != RC_SUCCESS)
	{
		pCDlgOta->m_pLog->Log("Send Req[0x%X] Failed, errCode=%d.\n", pReqPkt->m_Cmd, errCode);
		pCDlgOta->DownloadDone();
		return ;
	}

	if(REQ_OTA_START == pReqPkt->m_Cmd)
	{
		pCDlgOta->UpdateProcessCtrl(DOWNLOAD_START, (UINT)pCDlgOta->m_dwOtaFileLen);

		pCDlgOta->PostMessage(WM_OTA_SEND, REQ_OTA_UPDATE, 0);
		pCDlgOta->m_pLog->Log(L"Send REQ_OTA_UPDATE.\n");
	}
	else if(REQ_OTA_UPDATE == pReqPkt->m_Cmd)
	{
		DWORD offset = AS_UINT32(pReqPkt->m_Data[3], pReqPkt->m_Data[2], pReqPkt->m_Data[1], pReqPkt->m_Data[0]);

		offset += pReqPkt->m_Len - 4 - MAX_PAYLOAD_HEAD_SIZE;

		pCDlgOta->m_pLog->Log(L"Send offset[0x%04x].\n", offset);
		pCDlgOta->UpdateProcessCtrl(DOWNLOAD_POS, offset);

		if(offset < pCDlgOta->m_dwOtaFileLen)
		{
			pCDlgOta->PostMessage(WM_OTA_SEND, REQ_OTA_UPDATE, offset);
		}
		else
		{
			pCDlgOta->PostMessage(WM_OTA_SEND, REQ_OTA_COMPLETE, 0);
		}
	}
	else if(REQ_OTA_COMPLETE == pReqPkt->m_Cmd)
	{
		pCDlgOta->PostMessage(WM_OTA_SEND, REQ_OTA_VERIFY, 0);
	}
	else if(REQ_OTA_VERIFY == pReqPkt->m_Cmd)
	{
		pCDlgOta->DownloadDone();
		pCDlgOta->m_pLog->Log(L"Download success.\n");

		CTime endTime = CTime::GetCurrentTime();
		CTimeSpan ts = endTime - pCDlgOta->m_StartTime;
		pCDlgOta->m_pLog->Log(L"Total time: %ds\n", ts.GetTotalSeconds());
	}
}


IMPLEMENT_DYNAMIC(CDlgOta, CTabDlg)

CDlgOta::CDlgOta(CLog *pLog, CWnd* pParent /*=NULL*/)
	: CTabDlg(pLog, CDlgOta::IDD, pParent)
{
	g_pUtpMgr = new UtpMgr;
	g_dwDebugLevel |= DL_MAIN;
	g_dwDebugLevel |= DL_ERROR;
	g_dwDebugLevel |= DL_WARNING;
	g_dwDebugLevel |= DL_ASSERT;
//	g_dwDebugLevel |= DL_UTP;
	UtpMgr_Init(g_pUtpMgr
		, Utp_Tx
		, NULL
		, Utp_RspProc
		, this
		, 0
		);

	m_dwOtaFileLen = 0;
}

CDlgOta::~CDlgOta()
{
	if(g_pUtpMgr)
	{
		delete g_pUtpMgr;
		g_pUtpMgr = NULL;
	}

	DownloadDone();
}

void CDlgOta::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_BIN_FILE	, m_BinFileName);
	DDX_Control(pDX, IDC_EDIT_MTP		, m_OtaFilePath);

	DDX_Control(pDX,IDC_EDIT_MAIN_VERSION	, m_MainVerCtrl);
	DDX_Control(pDX,IDC_EDIT_MINOR_VER		, m_MinorVerCtrl);
	DDX_Control(pDX,IDC_EDIT_REV			, m_RevisionNumCtrl);
	DDX_Control(pDX,IDC_EDIT_BUILDNUM		, m_BuildNumCtrl);
	DDX_Control(pDX,IDC_EDIT_OTA_FILENAME	, m_OtaFileNameCtrl);

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


BEGIN_MESSAGE_MAP(CDlgOta, CTabDlg)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE, &CDlgOta::OnBnClickedButtonGenerate)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CDlgOta::OnBnClickedButtonDownload)
	ON_BN_CLICKED(IDC_BTN_BIN, &CDlgOta::OnBnClickedBtnBin)
	ON_BN_CLICKED(IDC_BTN_MTP, &CDlgOta::OnBnClickedBtnMtp)

	ON_WM_TIMER()
	ON_MESSAGE(WM_OTA_SEND	,&CDlgOta::OtaSend)
	ON_MESSAGE(WM_UTP_RUN	,&CDlgOta::UtpRun)
END_MESSAGE_MAP()

BOOL CDlgOta::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	m_BinFileName .Init(&g_Profile, L"FilePath", L"BinFileName" , L"");
	m_OtaFilePath .Init(&g_Profile, L"FilePath", L"OtaFilePath" , L".\\");

	m_MainVerCtrl	 .Init(&g_Profile, L"Setting", L"MainVer"	, L"1");
	m_MinorVerCtrl	 .Init(&g_Profile, L"Setting", L"MinorVer"	, L"0");
	m_RevisionNumCtrl.Init(&g_Profile, L"Setting", L"RevNum"	, L"0");
	m_BuildNumCtrl	 .Init(&g_Profile, L"Setting", L"BuildNum"	, L"1");
	m_OtaFileNameCtrl.Init(&g_Profile, L"FilePath", L"OtaFileName", L"BleFwForOta");

	UpdateUI();

	m_TimerID = SetTimer(0,1,0);

	return TRUE;
}
// CDlgOta 消息处理程序
void CDlgOta::SelChanged(CTabDlg* pNewTabDlg)
{
	m_BinFileName	 .Reflash();

	m_MainVerCtrl	 .Reflash();
	m_MinorVerCtrl	 .Reflash();
	m_RevisionNumCtrl.Reflash();
	m_BuildNumCtrl	 .Reflash();

	UpdateUI();
}

void CDlgOta::OnBnClickedButtonGenerate()
{
	if(!UpdateData())
	{
		return;
	}

	m_MainVerCtrl	 .Save();
	m_MinorVerCtrl	 .Save();
	m_RevisionNumCtrl.Save();
	m_BuildNumCtrl	 .Save();
	m_OtaFileNameCtrl.Save();

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
	strOtaFileName.Format(L"%s_%d.%d.%d.%d.bin"
		, m_OtaFileNameCtrl.GetString()
		, m_MainVerCtrl.GetValue()
		, m_MinorVerCtrl.GetValue()
		, m_RevisionNumCtrl.GetValue()
		, m_BuildNumCtrl.GetValue()
		);

	CString strOtaFullFileName;
	strOtaFullFileName.Format(L"%s\\%s", m_OtaFilePath.GetString(), strOtaFileName);

	int iRet = otaFile.ConvertCrcedFile(m_MainVerCtrl.GetValue()
		, m_MinorVerCtrl.GetValue()
		, m_RevisionNumCtrl.GetValue()
		, m_BuildNumCtrl.GetValue()
		, strOtaFullFileName
		);

	LOG(L"Convert to Ota format[ %s ] %s.\n", strOtaFileName, iRet ? L"SUCCESS" : L"FAILED");
}

void CDlgOta::OnBnClickedButtonDownload()
{
	if(m_dwOtaFileLen)
	{
		m_isUpdate = FALSE;
		//MessageBox(L"Download is in process", L"Warning", MB_OK | MB_ICONSTOP);
		return;
	}

	if(!UpdateData())
	{
		return;
	}

	COtaFile otaFile;
	if(!otaFile.Open(m_BinFileName.GetString(), CFile::modeRead))
	{
		LOG(L"Open file[%s] failed.\n", m_BinFileName.GetString());
	}

	BOOL isTemp = FALSE;
	m_strOtaFullFileName = m_BinFileName.GetString();
	if(!otaFile.IsCrcedFile())
	{
		isTemp = TRUE;
		m_strOtaFullFileName.Format(L"%s\\Temp.bin", m_OtaFilePath.GetString());
		int iRet = otaFile.ConvertCrcedFile(m_MainVerCtrl.GetValue()
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
	}
	otaFile.Close();

	if(!m_Otafile.Open(m_strOtaFullFileName, CFile::modeRead))
	{
		LOG(L"Open file[%s] failed.\n", m_strOtaFullFileName);
		return ;
	}

	//重定向接收函数
	CMainDlg* pDlg = (CMainDlg*)AfxGetMainWnd();
	pDlg->RegsterRcvDataCb(this, (HandlerFn)Utp_RxDataHandler);

	m_isUpdate = TRUE;
	m_dwOtaFileLen = (DWORD)m_Otafile.GetLength();

	//Ota start
	BYTE buff[10] = {0};
	int i = 0;
	buff[i++] = (BYTE)m_dwOtaFileLen;
	buff[i++] = (BYTE)(m_dwOtaFileLen >> 8);
	buff[i++] = (BYTE)(m_dwOtaFileLen >> 16);
	buff[i++] = (BYTE)(m_dwOtaFileLen >> 24);
	
	buff[i++] = 0;

	m_pLog->Log(L"Send REQ_OTA_START.\n");
	m_StartTime = CTime::GetCurrentTime();
	UpdateProcessCtrl(DOWNLOAD_START, (UINT)m_dwOtaFileLen);
	UtpMgr_PostReq(g_pUtpMgr, REQ_OTA_START, buff, i, RE_TX_COUNT);

	if(!isTemp)
	{
		m_strOtaFullFileName = L"";
	}
}

BOOL CDlgOta::DownloadOtaFile(CFile& file, DWORD offset)
{
	int i = 0;
	UINT readLen = 0;
	BYTE buff[256] = {0};

	buff[i++] = (BYTE)offset;
	buff[i++] = (BYTE)(offset>>8);
	buff[i++] = (BYTE)(offset>>16);
	buff[i++] = (BYTE)(offset>>24);

	readLen = (m_dwOtaFileLen - offset > MAX_UART_PURE_DATA_SIZE) ? MAX_UART_PURE_DATA_SIZE : (m_dwOtaFileLen - offset);
//	m_pLog->Log(L"Send REQ_OTA_UPDATE %04x/%04x.\n", offset, m_dwOtaFileLen);

	file.Seek(offset, 0);
	if(!file.Read(&buff[i], readLen))
	{
		LOG(L"Read file[%d][%d] failed.\n", offset, readLen);
		return FALSE;
	}
	i += readLen;

	UtpMgr_PostReq(g_pUtpMgr, REQ_OTA_UPDATE, buff, i, RE_TX_COUNT);

	return TRUE;
}

void CDlgOta::DownloadDone()
{
	if(m_dwOtaFileLen)
	{
		m_dwOtaFileLen = 0;
		UpdateProcessCtrl(DOWNLOAD_END);
	}

	if(m_Otafile.m_hFile != (HANDLE)0xFFFFFFFF)
	{
		m_Otafile.Close();
	}

	CFileFind find;
	if(m_strOtaFullFileName.GetLength() && find.FindFile(m_strOtaFullFileName))
	{
		CFile file;
		file.Remove(m_strOtaFullFileName);
		m_strOtaFullFileName = L"";
	}

	if(::IsWindow(m_hWnd) && m_TimerID)
	{
		KillTimer(m_TimerID);
		m_TimerID = 0;
	}
}

void CDlgOta::OnBnClickedBtnBin()
{
	m_BinFileName.OpenFileDlg(_T("Binary File (*.bin)|*.bin|"));

	UpdateUI();
}

void CDlgOta::OnBnClickedBtnMtp()
{
	m_OtaFilePath.OpenFolderDlg();
}

void CDlgOta::OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam)
{
	if(eEvent == EVENT_USB_PLUG)
	{
		GetDlgItem(IDC_BUTTON_DOWNLOAD)->EnableWindow(param);
	}
	else if(eEvent == EVENT_DOWNLOAD_PROCESS)
	{
		if(param == DOWNLOAD_START)
		{
			//GetDlgItem(IDC_BTN_BIN)->EnableWindow(FALSE);
			m_BtnDownLoad.EnableWindow(FALSE);
		}
		else if(param == DOWNLOAD_END)
		{
			m_dwOtaFileLen = 0;
			DownloadDone();
			//GetDlgItem(IDC_BTN_BIN)->EnableWindow(TRUE);
			m_BtnDownLoad.EnableWindow(TRUE);
		}
	}
}

void CDlgOta::UpdateProcessCtrl(DWORD msg, UINT pos)
{
	::AfxGetMainWnd()->PostMessage(WM_DOWNLOAD, msg, pos);
	if(DOWNLOAD_END == msg)
	{
		CMainDlg* pDlg = (CMainDlg*)AfxGetMainWnd();
		pDlg->RegsterRcvDataCb(NULL, NULL);
	}
}

void CDlgOta::UpdateUI()
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

void CDlgOta::OnTimer(UINT nIDEvent)
{
	PostMessage(WM_UTP_RUN, 0, 0);
	CDialogEx::OnTimer(nIDEvent);
}


LRESULT CDlgOta::OtaSend(WPARAM wParam, LPARAM lParam)
{
	UINT offset = (WPARAM)lParam;
	

	if(REQ_OTA_UPDATE == wParam)
	{
		if(!DownloadOtaFile(m_Otafile, offset))
		{
			m_pLog->Log("Download OTA file[%d] failed.\n", offset);
			DownloadDone();
			return 0;
		}
	}
	else if(REQ_OTA_COMPLETE == wParam)
	{
		m_pLog->Log(L"Send REQ_OTA_COMPLETE.\n");
		UtpMgr_PostReq(g_pUtpMgr, REQ_OTA_COMPLETE, NULL, 0, RE_TX_COUNT);
	}
	else if(REQ_OTA_VERIFY == wParam)
	{
		m_pLog->Log(L"Send REQ_OTA_VERIFY.\n");
		UtpMgr_PostReq(g_pUtpMgr, REQ_OTA_VERIFY, NULL, 0, RE_TX_COUNT);
	}

	return 0;
}

LRESULT CDlgOta::UtpRun(WPARAM wParam, LPARAM lParam)
{
	UtpMgr_Run(g_pUtpMgr);

	return 0;
}