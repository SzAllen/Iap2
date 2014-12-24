
// CodeTester.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#define CHECK_STRING_RESET() theApp.StringReset()

// CCodeTesterApp:
// 有关此类的实现，请参阅 CodeTester.cpp
//

class CCodeTesterApp : public CWinApp
{
public:
	CString m_ProfileName;
	CHAR m_WgIp[20];
	CHAR m_CdmsIp[20];
	CHAR m_BcIp[20];
	CHAR m_IpMask[20];
	CHAR m_IpGate[20];

	CCodeTesterApp();
	CString m_LogString;
public:
	CCriticalSection	m_CritSection;

// 重写
public:
	void PrintfToString(const CString& str);
	void PrintfToString(LPCTSTR pStr);
	void PrintfToString(const char* pStr);
	void StringReset();
	int FindSubStrCount(const CString& subSstr, Bool isAutoClear);
	int FindSubStrCount(const char* str, Bool isAutoClear);

	virtual BOOL InitInstance();

// 实现
	afx_msg void OnTesterMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

extern CCodeTesterApp theApp;
void MyLock();
void MyUnlock();
UINT CmTimerThread(LPVOID pVoid);
void mmsleep(int times);
extern CCodeTesterApp theApp;

