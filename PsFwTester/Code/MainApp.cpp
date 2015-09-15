
// Tester.cpp : ����Ӧ�ó��������Ϊ��
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


// CMainApp ����

CMainApp::CMainApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	TraceEx = (PrintfFun)MyTrace;	//MyTrace LogEx

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMainApp ����

CMainApp theApp;

// CMainApp ��ʼ��

BOOL CMainApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	g_Profile.Init(theApp);

	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	_tsetlocale(LC_CTYPE, _T("chs"));
	AfxInitRichEdit2();


	CMainDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
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