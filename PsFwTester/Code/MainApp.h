
// MainApp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "CaseTreeData.h"

// CMainApp:
// �йش����ʵ�֣������ Tester.cpp
//

class CMainApp : public CWinApp
{
public:
	CCriticalSection	m_CritSection;
public:
	CMainApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	DECLARE_MESSAGE_MAP()
};

extern CMainApp theApp;
CCaseTreeData* GetTreeData();
CCaseTreeData* GetInputTreeData();

int MyTrace(LPSTR lpszFormat, ...);
int LogEx(LPSTR lpszFormat, ...);

