#pragma once

#include "CaseTreeData.h"
#include "Tester.h"

class CArchTester :	public CTester
{
	HANDLE m_hRxThread;
	HANDLE m_hArchThread;
public:
	CArchTester(void);
	~CArchTester(void);
public:
	//≤‚ ‘”√¿˝
};

extern CArchTester g_ArchTester;