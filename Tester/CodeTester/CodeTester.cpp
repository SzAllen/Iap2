
// CodeTester.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "CodeTester.h"
#include "CodeTesterDlg.h"
#include "CaseTreeData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEmmcTesterApp 构造
#pragma warning (push)
#pragma warning(disable : 4075)
	#pragma init_seg(".CRT$XCC")
#pragma warning (pop)
CCaseTreeData	g_TestCaseTreeData;
CMutex			g_Mutex;
// 唯一的一个 CCodeTesterApp 对象
CCodeTesterApp theApp;

BEGIN_MESSAGE_MAP(CCodeTesterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_THREAD_MESSAGE(ID_RUN_CMTESTER, &CCodeTesterApp::OnTesterMsg)
END_MESSAGE_MAP()


// CCodeTesterApp 构造

CCodeTesterApp::CCodeTesterApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

	TraceEx = (PrintfFun)LogEx;	//MyTrace LogEx
	//Lock = MyLock;
	//Unlock = MyUnlock;
	//mSleep = (tSleep)mmsleep;

}




// CCodeTesterApp 初始化

BOOL CCodeTesterApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	_tsetlocale(LC_CTYPE, _T("chs"));
	AfxInitRichEdit2();

	::AfxSocketInit();

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	WCHAR buff[100];
	GetCurrentDirectory(100, buff);
	m_ProfileName = buff;
	m_ProfileName += "\\";
	m_ProfileName += m_pszProfileName;

	WIN32_FIND_DATAW findFileData;
	/*
	uint32 value1 = 0x10000000;
	TRACE(L"value1=%08x\n", value1);
	uint32 value2  = value1 << 4;

	TRACE("value=%08x\n", value2);
	uint32 value3 = value1 - value2;
	TRACE("value1 - value2=%08x\n", value1 - value2);
	TRACE("value2 - value1=%08x\n", value2 - value1);
	if(value1 >= 0x0FFFFFFF)
	{
		value1 = (0xFFFFFFFF - value1) << 4;
	}
	*/
	HANDLE handle = FindFirstFile(m_ProfileName, &findFileData);
	if(0xFFFFFFFF ==  (UINT)handle)
	{
		::WritePrivateProfileString(L"FrameInfo",L"CarrierNum",L"10",m_ProfileName);
		::WritePrivateProfileString(L"FrameInfo",L"SiteNum",L"1",m_ProfileName);
		::WritePrivateProfileString(L"FrameInfo",L"GwID",L"1",m_ProfileName);
		::WritePrivateProfileString(L"FrameInfo",L"GwType",L"31",m_ProfileName);
		::WritePrivateProfileString(L"FrameInfo",L"CpuID",L"1",m_ProfileName);

		::WritePrivateProfileString(L"WirelessGate",L"WgIpAddr",L"192.168.0.27",m_ProfileName);
		::WritePrivateProfileString(L"WirelessGate",L"CdmsIpAddr",L"192.168.0.25",m_ProfileName);
		::WritePrivateProfileString(L"WirelessGate",L"BcIpAddr",L"192.168.0.20",m_ProfileName);
		::WritePrivateProfileString(L"WirelessGate",L"IpMask",L"255.255.255.0",m_ProfileName);
		::WritePrivateProfileString(L"WirelessGate",L"IpGate",L"192.168.0.1",m_ProfileName);

		::WritePrivateProfileString(L"CmdList",L"Cmd01",L"0",m_ProfileName);
	}

	CStringA strName(m_ProfileName);
//	CHAR buf[100];
	DWORD ipaddr = 0;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	if (!AfxSocketInit())
	{
		AfxMessageBox(L"Socket init failed.");
		return FALSE;
	}

	//GetPrivateProfileString(L"wg.ini", L"

	CCodeTesterDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。

	return FALSE;
}


void CCodeTesterApp::OnTesterMsg(WPARAM wParam, LPARAM lParam)
{
	g_TestCaseTreeData.OnMsgProc( wParam, lParam);
}

void CCodeTesterApp::PrintfToString(const CString& subStr)
{
	m_CritSection.Lock();
	if(m_LogString.GetLength() <= 1024 * 1024)
	{
		m_LogString += subStr;
	}
	else
	{
		m_LogString = L"";
	}
	m_CritSection.Unlock();
}


void CCodeTesterApp::PrintfToString(LPCTSTR pStr)
{
	m_CritSection.Lock();
	if(m_LogString.GetLength() <= 1024 * 1024)
	{
		m_LogString += pStr;
	}
	else
	{
		m_LogString = L"";
	}
	m_CritSection.Unlock();
}

void CCodeTesterApp::PrintfToString(const char* pStr)
{
	m_CritSection.Lock();
	if(m_LogString.GetLength() <= 1024 * 1024)
	{
		m_LogString += CString(pStr);
	}
	else
	{
		m_LogString = L"";
	}
	m_CritSection.Unlock();
}

void CCodeTesterApp::StringReset()
{
	m_CritSection.Lock();
	m_LogString = L"";
	m_CritSection.Unlock();
}

int CCodeTesterApp::FindSubStrCount(const char* string, Bool isAutoClear)
{
	CString str(string);
	int count =  FindSubStrCount(str, isAutoClear);
	return count;
}

int CCodeTesterApp::FindSubStrCount(const CString& subStr, Bool isAutoClear)
{
	m_CritSection.Lock();
	int index = 0;
	int count = 0;

	int strLen = m_LogString.GetLength();
	int i = 0;
	for(i = 0; i < strLen; )
	{
		index = m_LogString.Find(subStr,i);
		if(index >= 0)
		{
			WCHAR ch = '"';
			if(index > 0)
			{
				if(m_LogString[index-1] != ch)
				{
					count++;
				}
			}
			else
			{
				count++;
			}
		}
		else
		{
			break;
		}
		i = index + subStr.GetLength();
	}

	if(isAutoClear)
	{
		m_LogString.Delete(0, i ? i : strLen);
	}

	m_CritSection.Unlock();
	return count;
}


UINT CmTimerThread(LPVOID pVoid)
{
	//uint32 test1,test2 = 1;
	DWORD g_Ticks = 5;
	while(1)
	{
		Sleep(g_Ticks);
		//Timermanager_Run(&g_TimerManager,  GetTickCount());
	}
}

int Log(LPCTSTR lpStr)
{
	CCodeTesterDlg* pDlg= (CCodeTesterDlg*)theApp.GetMainWnd();
	pDlg->m_OutPutCtrl.OutPut(lpStr);

	return wcslen(lpStr);
}

int Log(char* pStr)
{
	CCodeTesterDlg* pDlg= (CCodeTesterDlg*)theApp.GetMainWnd();
	if(pDlg)
	{
		theApp.m_CritSection.Lock();
		CString str(pStr);
		pDlg->m_OutPutCtrl.OutPut(str);
		theApp.m_CritSection.Unlock();

		return str.GetLength();
	}

	return 0;
}

int LogEx(LPSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	char pszBuffer[1024];
#pragma warning (push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)
	vsprintf( pszBuffer, pszFormat, ptr );
#pragma warning (pop)
	va_end(ptr);

	theApp.PrintfToString(pszBuffer);

	CCodeTesterDlg* pDlg= (CCodeTesterDlg*)theApp.GetMainWnd();
	if(pDlg)
	{
		theApp.m_CritSection.Lock();
		CString str(pszBuffer);
		pDlg->m_OutPutCtrl.OutPut(str);
		theApp.m_CritSection.Unlock();

		return str.GetLength();
	}

	return 0;
}

int Log(LPCTSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	wchar_t pszBuffer[1024];
#pragma warning (push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)
	vswprintf( pszBuffer, 1024, pszFormat, ptr );
#pragma warning (pop)
	va_end(ptr);

	theApp.PrintfToString(pszBuffer);

	CCodeTesterDlg* pDlg= (CCodeTesterDlg*)theApp.GetMainWnd();
	if(pDlg)
	{
		theApp.m_CritSection.Lock();
		pDlg->m_OutPutCtrl.OutPut(pszBuffer);
		theApp.m_CritSection.Unlock();
		return wcslen(pszBuffer);
	}

	return 0;
}

int MyTrace(LPSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	char szBuffer[512];

#pragma warning (push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)
	nBuf = vsprintf( szBuffer, lpszFormat, args );
	//nBuf = _vstprintf_s(szBuffer, _countof(szBuffer), lpszFormat, args); 
#pragma warning (pop)
	// was there an error? was the expanded string too long?
	ASSERT(nBuf >= 0);
	theApp.PrintfToString(szBuffer);

#ifdef NDEBUG
#else
	afxDump << szBuffer;
#endif
	va_end(args);

	return nBuf;
}

void MyLock()
{
	g_Mutex.Lock();
}

void MyUnlock()
{
	g_Mutex.Unlock();
}

void mmsleep(int times)
{
	Sleep(times);
}
