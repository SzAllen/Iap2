#include "StdAfx.h"
#include "TreeNodeData.h"


void TreeNodeData_Dump(TreeNodeData* pTreeNodeData)
{
	TRACE2("Level=%02d,Name=%s\n",pTreeNodeData->m_Level,pTreeNodeData->m_pName);
}
