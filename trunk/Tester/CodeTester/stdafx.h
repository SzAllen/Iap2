
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����

#include <afxsock.h>		//Socket 

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

//////////////////////////////////////////////////

#include <locale.h>

extern "C" 
{
	#include "typedef.h"
	#include "Debug.h"
	#include "CmdLine.h"
	#include "Shell.h"
}

extern int MyTrace(LPSTR lpszFormat, ...);
extern int LogEx(LPSTR pszFormat, ...);
extern int Log(LPCTSTR pszFormat, ...);
extern int Log(char* pStr);
extern CMutex g_Mutex;
#define LOG(x) Log x 

#define CHECK_STRING_RESET() theApp.StringReset()

#define CHECK_STRING2(maxMs, string, isAutoClear) {	\
	int ms = 0; \
	while(theApp.FindSubStrCount(string, isAutoClear) == 0)	\
	{	\
		Sleep(1);	\
		if(ms++ >= maxMs)	\
		break;	\
	}	\
	if(ms < maxMs)	\
	{	\
		Trace("Test Passed: �����ַ��� %s\n", #string);	\
	}	\
	else	\
	{	\
		Trace("*Test Failed*: û�з����ַ��� %s\n", #string);	\
		ASSERT(FALSE);	\
	}\
}

#define CHECK_STRING(string) CHECK_STRING2(10000, string, TRUE)

#define SAFE_RELEASE_THREAD(thread) \
	if(thread)	\
	{	\
		TerminateThread(thread->m_hThread, 0);	\
		delete thread;	\
		thread = NULL;	\
	}

////////////////////////////////////////////////

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


