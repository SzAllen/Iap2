#include "StdAfx.h"
#include "Tester.h"
#include "CaseTreeData.h"

#define Trace(...);

CTester::CTester(void)
{
}

CTester::CTester(TreeNodeData nodeData[], int nCount)
{
	GetTreeData()->AddUnitTreeNodeData(nodeData, nCount);
}

CTester::~CTester(void)
{
}

void CTester::PrintTreeNodeData(TreeNodeData* pTreeNodeData)
{
	Trace("\n测试用例：%s\n", pTreeNodeData->m_pName);
	/*
	if(pTreeNodeData->m_lpDesc)
	{
		Trace("测试说明：%s\n", pTreeNodeData->m_lpDesc);
	}
	*/
}

void CTester::PrintTestResult(TreeNodeData* pTreeNodeData, BOOL isPass)
{
	if(isPass)
	{
		Trace("测试结果：成功\n\n");
	}
	else
	{
		Trace("测试结果：失败\n\n");
	}
}

