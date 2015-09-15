
// Tester.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MainApp.h"
#include "MainDlg.h"
#include "Profile.h"
#include "CaseTreeData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainApp

BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMainApp 构造

CMainApp::CMainApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	TraceEx = (PrintfFun)MyTrace;	//MyTrace LogEx

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMainApp 对象

CMainApp theApp;

// CMainApp 初始化

BOOL CMainApp::InitInstance()
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

	g_Profile.Init(theApp);

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	_tsetlocale(LC_CTYPE, _T("chs"));
	AfxInitRichEdit2();


	CMainDlg dlg;
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

void MsgBox(LPCTSTR pszFormat, ...)
{
	CWnd* pWnd = ::AfxGetMainWnd();

	va_list ptr;
	va_start(ptr, pszFormat);
	wchar_t pszBuffer[128];

	vswprintf( pszBuffer, 1024, pszFormat, ptr );

	va_end(ptr);

	MessageBox(pWnd->m_hWnd, pszBuffer, L"Infomation", MB_OK | MB_ICONEXCLAMATION);
}

int MyTrace(LPSTR lpszFormat, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	char szBuffer[512]={0};

	nBuf = vsprintf( szBuffer, lpszFormat, args );
	//afxDump << szBuffer;
	va_end(args);

	LPCTSTR lpStr = CString(szBuffer);
	SendMessage(theApp.m_pMainWnd->m_hWnd, WM_LOG, (WPARAM)szBuffer, 1);
	//MsgBox();

	return nBuf;
#endif
	return 0;
}

int LogEx(LPSTR lpszFormat, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	char szBuffer[512]={0};

	nBuf = vsprintf( szBuffer, lpszFormat, args );
	va_end(args);

	afxDump << szBuffer;
	//MsgBox();

	return nBuf;
#endif
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

	CMainDlg* pDlg= (CMainDlg*)theApp.GetMainWnd();
	if(pDlg)
	{
		theApp.m_CritSection.Lock();
		pDlg->Log(pszBuffer);
		theApp.m_CritSection.Unlock();
		return wcslen(pszBuffer);
	}

	return 0;
}

CCaseTreeData* GetTreeData()
{
   static CCaseTreeData treeData;
   return &treeData;
}

CCaseTreeData* GetInputTreeData()
{
   static CCaseTreeData treeData;
   return &treeData;
}