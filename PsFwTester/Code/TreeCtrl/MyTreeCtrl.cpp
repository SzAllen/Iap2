// MyTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTreeCtrl.h"


// CMyTreeCtrl

IMPLEMENT_DYNAMIC(CMyTreeCtrl, CTreeCtrl)

CMyTreeCtrl::CMyTreeCtrl()
{

}

CMyTreeCtrl::~CMyTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()

SEARCH_FALG CMyTreeCtrl::SeatchItem(HTREEITEM hItem, OperateFun Operate, DWORD param)
{
	HTREEITEM hNextItem = hItem;
	if(hItem)
	{
		hNextItem = GetNextItem(hItem,TVGN_CHILD);
		if(FLAG_BREAK == Operate(this, hItem, hNextItem != NULL, param)) return FLAG_BREAK;

		if(hNextItem)
		{
			if(FLAG_BREAK == SeatchItem(hNextItem, Operate, param)) return FLAG_BREAK;
		}
	}

	hItem = GetNextItem(hItem,TVGN_NEXT);
	while(hItem)
	{
		hNextItem = GetNextItem(hItem,TVGN_CHILD);

		if(FLAG_BREAK == Operate(this, hItem, hNextItem != NULL, param)) return FLAG_BREAK;

		if(hNextItem)
		{
			if(FLAG_BREAK == SeatchItem(hNextItem, Operate, param)) return FLAG_BREAK;
		}

		hItem = GetNextItem(hItem,TVGN_NEXT);
	}
	return FLAG_CONTINUE;
}

HTREEITEM CMyTreeCtrl::FindItem(HTREEITEM hItem, void* pItemData)
{
	HTREEITEM hNextItem = hItem;
	if(hItem)
	{
		if((void*)GetItemData(hItem) == pItemData) return hItem;

		hNextItem = GetNextItem(hItem,TVGN_CHILD);

		if(hNextItem)
		{
			HTREEITEM retTreeItem = FindItem(hNextItem, pItemData);
			if(retTreeItem) return retTreeItem;
		}
	}
	else
	{
		return NULL;
	}

	hItem = GetNextItem(hItem,TVGN_NEXT);
	while(hItem)
	{
		if((void*)GetItemData(hItem) == pItemData) return hItem;

		hNextItem = GetNextItem(hItem,TVGN_CHILD);
		if(hNextItem)
		{
			HTREEITEM retTreeItem = FindItem(hNextItem, pItemData);
			if(retTreeItem) return retTreeItem;
		}

		hItem = GetNextItem(hItem,TVGN_NEXT);
	}
	return NULL;
}

SEARCH_FALG Expend(CTreeCtrl* pTreeCtrl, HTREEITEM hItem, BOOL isParent, DWORD param)
{
	//TreeNodeData* pTreeNodeData = (TreeNodeData*)pTreeCtrl->GetItemData(hItem);
	//if(hItem && isParent && pTreeNodeData && pTreeNodeData->m_Level < 2)
	if(hItem && isParent)
	{
		pTreeCtrl->Expand(hItem, param);
	}
	return FLAG_CONTINUE;
}

void CMyTreeCtrl::ExpendAll(HTREEITEM hItem, BOOL isExpend)
{
	SeatchItem(hItem, (OperateFun)Expend, TVE_EXPAND);
}

// CMyTreeCtrl 消息处理程序


