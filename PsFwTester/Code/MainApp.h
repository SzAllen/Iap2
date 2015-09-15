
// MainApp.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "CaseTreeData.h"

// CMainApp:
// 有关此类的实现，请参阅 Tester.cpp
//

class CMainApp : public CWinApp
{
public:
	CCriticalSection	m_CritSection;
public:
	CMainApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现
	DECLARE_MESSAGE_MAP()
};

extern CMainApp theApp;
CCaseTreeData* GetTreeData();
CCaseTreeData* GetInputTreeData();

int MyTrace(LPSTR lpszFormat, ...);
int LogEx(LPSTR lpszFormat, ...);

