#include "StdAfx.h"
#include "CaseTreeData.h"

////////////////////////////////////////////////////////////////////////////////

//CmdParser g_CmdParser;
CCaseTreeData::CCaseTreeData()
{
	static TreeNodeData g_UnitTreeNodeData	= {0, _T("单元测试用例")};
	static TreeNodeData g_IntegTreeNodeData = {0, _T("集成测试用例"), (TesterFun)NULL};

	m_nUnitTreeNodeDatas = 0;
	memset(m_pUnitTreeNodeDatas, 0, sizeof(m_pUnitTreeNodeDatas));
	m_pUnitTreeNodeDatas[m_nUnitTreeNodeDatas++] = &g_UnitTreeNodeData;

	m_nIntegTreeNodeDatas = 0;
	memset(m_pIntegTreeNodeDatas, 0, sizeof(m_pIntegTreeNodeDatas));
	m_pIntegTreeNodeDatas[m_nIntegTreeNodeDatas++] = &g_IntegTreeNodeData;


	memset(m_pTesters, 0, sizeof(m_pTesters));

	//CmdParser_Init(&g_CmdParser,)
}

CCaseTreeData::~CCaseTreeData()
{
}

void CCaseTreeData::Dump()
{
	//TRACE3("Name=%s,NodeId=%d,ParentNodeId=%d\n",m_pName,m_NodeId,m_Level);
}

void CCaseTreeData::AddTester(CTester* pTester)
{
	for(int i = 0; i < MAX_TESTER; i++)
	{
		if(NULL == m_pTesters[i])
		{
			m_pTesters[i] = pTester;
			return;
		}
	}
	ASSERT(FALSE);
}

void CCaseTreeData::OnMsgProc(WPARAM wParam, LPARAM lParam)
{
	for(int i = 0; i < MAX_TESTER; i++)
	{
		if((LPARAM)m_pTesters[i] == lParam)
		{
			m_pTesters[i]->OnMsgProc(wParam);
		}
	}
}

/*
int CTestCase_ExcuteFun(CmdParser* pCmdParser, CmdLine* pCmdLine)
{
	for(int i = 0; i < GetTreeData()->m_nUnitTreeNodeDatas; i++)
	{
		if(pCmdLine == &GetTreeData()->m_CmdLine[i])
		{
			GetTreeData()->m_pUnitTreeNodeDatas[i]->Run(GetTreeData()->m_pUnitTreeNodeDatas[i]);
		}
	}
	
	return 0;
}
*/
void CCaseTreeData::AddUnitTreeNodeData(TreeNodeData TreeNodeData[], int count)
{
	for(int i = 0; i < count; i++)
	{
		ASSERT(m_nUnitTreeNodeDatas < MAX_UNIT_TESTCASE);

		//m_CmdLine[m_nUnitTreeNodeDatas].m_pCmdParser = (char*)TreeNodeData[i].m_pName;
		//m_CmdLine[m_nUnitTreeNodeDatas].CmdProc = CTestCase_ExcuteFun;
		//m_CmdLine[m_nUnitTreeNodeDatas].m_pHelp = (char*)TreeNodeData[i].m_lpDesc;

		m_pUnitTreeNodeDatas[m_nUnitTreeNodeDatas++] = &TreeNodeData[i];
	}
}

void CCaseTreeData::AddIntegTreeNodeData(TreeNodeData TreeNodeData[], int count)
{
	for(int i = 0; i < count; i++)
	{
		ASSERT(m_nIntegTreeNodeDatas < MAX_INTEG_TESTCASE);
		m_pIntegTreeNodeDatas[m_nIntegTreeNodeDatas++] = &TreeNodeData[i];
	}
}
