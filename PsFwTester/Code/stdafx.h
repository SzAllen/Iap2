
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



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#include <locale.h>


#define SAFE_DELETE(pObj) if(pObj) {delete pObj; pObj = NULL;}

#include "Profile.h"
#include "Log.h"

#define CHECK_STRING_RESET(...)
extern int Log(LPCTSTR pszFormat, ...);

extern "C"
{
	#include "Typedef.h"
	#include "UtpMgr.h"
	#include "Iap2Link.h"
	#include "Iap2CtrlSession.h"
	#include "Driver.h"

	extern Iap2Link* g_pIap2Link;
}

#define WM_LOG			(WM_USER + 1)
#define WM_DOWNLOAD		(WM_USER + 2)
#define WM_USB_PLUG_IN	(WM_USER + 3)
#define WM_USB_PLUG_OUT	(WM_USER + 4)
#define DOWNLOAD_START		0
#define DOWNLOAD_END		1
#define DOWNLOAD_POS		2

#define SAFE_RELEASE_THREAD(thread) \
	if(thread)	\
	{	\
		TerminateThread(thread->m_hThread, 0);	\
		delete thread;	\
		thread = NULL;	\
	}

extern void MsgBox(LPCTSTR pszFormat, ...); 
extern CProfile g_Profile;
extern LPCTSTR g_pComSettingSection;
extern LPCTSTR g_pComSetting[];

#define MY_RPTF(...)
#define MY_ASSERTE(...)

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


