#pragma once

#include "TreeNodeData.h"
#include "Tester.h"

enum
{
	  NODE_IMAGE = 1
	, NODE_SELECT_IMAGE = 2
};

#define SAFE_RELEASE_THREAD(thread) \
	if(thread)	\
	{	\
		TerminateThread(thread->m_hThread, 0);	\
		delete thread;	\
		thread = NULL;	\
	}

class CCaseTreeData
{
public:
	CCaseTreeData(void);
	~CCaseTreeData(void);
public:
	enum{MAX_UNIT_TESTCASE = 200};
	enum{MAX_INTEG_TESTCASE = 200};
	enum{MAX_TESTER = 50};

	int m_nUnitTreeNodeDatas;
	TreeNodeData* m_pUnitTreeNodeDatas[MAX_UNIT_TESTCASE];

	int m_nIntegTreeNodeDatas;
	TreeNodeData* m_pIntegTreeNodeDatas[MAX_INTEG_TESTCASE];

	CTester*	m_pTesters[MAX_TESTER];

	//CmdLine	m_CmdLine[MAX_UNIT_TESTCASE];
public:
	void AddTester(CTester* pTester);
	void OnMsgProc(WPARAM wParam, LPARAM lParam);

	void Dump();
	void AddUnitTreeNodeData(TreeNodeData TreeNodeData[], int count);
	void AddIntegTreeNodeData(TreeNodeData TreeNodeData[], int count);

	static UINT TimerThread(LPVOID pVoid);
private:

};

extern CCaseTreeData g_TestCaseTreeData;
