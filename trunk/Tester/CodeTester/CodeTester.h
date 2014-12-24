
// CodeTester.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#define CHECK_STRING_RESET() theApp.StringReset()

// CCodeTesterApp:
// �йش����ʵ�֣������ CodeTester.cpp
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

// ��д
public:
	void PrintfToString(const CString& str);
	void PrintfToString(LPCTSTR pStr);
	void PrintfToString(const char* pStr);
	void StringReset();
	int FindSubStrCount(const CString& subSstr, Bool isAutoClear);
	int FindSubStrCount(const char* str, Bool isAutoClear);

	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnTesterMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

extern CCodeTesterApp theApp;
void MyLock();
void MyUnlock();
UINT CmTimerThread(LPVOID pVoid);
void mmsleep(int times);
extern CCodeTesterApp theApp;

