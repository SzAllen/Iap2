
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类

#include <afxsock.h>		//Socket 

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

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
		Trace("Test Passed: 发现字符串 %s\n", #string);	\
	}	\
	else	\
	{	\
		Trace("*Test Failed*: 没有发现字符串 %s\n", #string);	\
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


