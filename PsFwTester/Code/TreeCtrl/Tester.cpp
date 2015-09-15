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
	Trace("\n����������%s\n", pTreeNodeData->m_pName);
	/*
	if(pTreeNodeData->m_lpDesc)
	{
		Trace("����˵����%s\n", pTreeNodeData->m_lpDesc);
	}
	*/
}

void CTester::PrintTestResult(TreeNodeData* pTreeNodeData, BOOL isPass)
{
	if(isPass)
	{
		Trace("���Խ�����ɹ�\n\n");
	}
	else
	{
		Trace("���Խ����ʧ��\n\n");
	}
}

