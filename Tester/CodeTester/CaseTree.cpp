// CaseTree.cpp : 实现文件
//

#include "stdafx.h"

#include "CaseTree.h"


// CCaseTree

IMPLEMENT_DYNAMIC(CCaseTree, CWnd)

CCaseTree::CCaseTree()
{

}

CCaseTree::~CCaseTree()
{
}


BEGIN_MESSAGE_MAP(CCaseTree, CWnd)
END_MESSAGE_MAP()

BOOL CCaseTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

BOOL CCaseTree::Init()
{
	return TRUE;
}
// CCaseTree 消息处理程序
SEARCH_FALG CCaseTree::SeatchItem(HTREEITEM hItem, OperateFun Operate, DWORD param)
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

HTREEITEM CCaseTree::FindItem(HTREEITEM hItem, void* pItemData)
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

SEARCH_FALG Expend(CCaseTree* pTreeCtrl, HTREEITEM hItem, BOOL isParent, DWORD param)
{
	TestItem* pTestItem = (TestItem*)pTreeCtrl->GetItemData(hItem);
	if(hItem && isParent && pTestItem && pTestItem->m_Level < 2)
	{
		pTreeCtrl->Expand(hItem, param);
	}
	return FLAG_CONTINUE;
}

void CCaseTree::ExpendAll(HTREEITEM hItem, BOOL isExpend)
{
	SeatchItem(hItem, (OperateFun)Expend, TVE_EXPAND);
}

void CCaseTree::FillTestItem(HTREEITEM pParent, TestItem* pTestItem[], int nCount, int& i, TestItem* pParentTestItem)
{
	HTREEITEM node = NULL;
	for(i = i; i < nCount; i++)
	{
		if(pParent)
		{
			//TestItem_Dump(pTestItem[i]);
			ASSERT(pParent && pParentTestItem);
			if(pTestItem[i]->m_Level == pParentTestItem->m_Level + 1)
			{
				node = InsertItem(pTestItem[i]->m_pName, 0, 0, pParent);
				SetItemData(node, (DWORD_PTR)pTestItem[i]);
				i++;
				FillTestItem(node, pTestItem, nCount, i, pTestItem[i-1]);
			}
			else
			{
				i--;
				return ;
			}
		}
		else if(pTestItem[i]->m_Level == 0)
		{
			//TestItem_Dump(pTestItem[i]);
			node = InsertItem(pTestItem[i]->m_pName, 0, 0);
			SetItemData(node, (DWORD_PTR)pTestItem[i]);
			i++;
			FillTestItem(node, pTestItem, nCount, i, pTestItem[i-1]);
		}
	}
}

void CCaseTree::FillTestItem()
{
	int i = 0;
	FillTestItem(NULL, g_CTestCase.m_pUnitTestItems,  g_CTestCase.m_nUnitTestItems, i, NULL);

	i = 0;
	FillTestItem(NULL, g_CTestCase.m_pIntegTestItems,  g_CTestCase.m_nIntegTestItems, i, NULL);

	HTREEITEM hItem = GetNextItem(NULL,TVGN_ROOT);
	ExpendAll(hItem, TRUE);
}

void CCaseTree::FillParamItem(HTREEITEM pParent, ParamItem* pParamItem[], int nCount, int& i, ParamItem* pParentParamItem)
{
	HTREEITEM node = NULL;
	for(i = i; i < nCount; i++)
	{
		if(pParent)
		{
			//ParamItem_Dump(pParamItem[i]);
			ASSERT(pParent && pParentParamItem);
			if(pParamItem[i]->m_Level == pParentParamItem->m_Level + 1)
			{
				node = InsertItem(pParamItem[i]->m_pName, 0, 0, pParent);
				SetItemData(node, (DWORD_PTR)pParamItem[i]);
				SetCheck(node, pParamItem[i]->m_isChecked);
				i++;
				FillParamItem(node, pParamItem, nCount, i, pParamItem[i-1]);
			}
			else
			{
				i--;
				return ;
			}
		}
		else if(pParamItem[i]->m_Level == 0)
		{
			//ParamItem_Dump(pParamItem[i]);
			node = InsertItem(pParamItem[i]->m_pName, 0, 0);
			SetItemData(node, (DWORD_PTR)pParamItem[i]);
			SetCheck(node, pParamItem[i]->m_isChecked);
			i++;
			FillParamItem(node, pParamItem, nCount, i, pParamItem[i-1]);
		}
	}
}
void CCaseTree::FillDeviceItem()
{
	int i = 0;
	FillParamItem(NULL, g_DeviceTree.m_pDeviceParams,  g_DeviceTree.m_nDeviceParamsCount, i, NULL);

	HTREEITEM hItem = GetNextItem(NULL,TVGN_ROOT);
	ExpendAll(hItem, TRUE);

//	UpdateNode(g_DeviceTree.m_pDeviceParams,  g_DeviceTree.m_nDeviceParamsCount);
}

SEARCH_FALG Update(CCaseTree* pTreeCtrl, HTREEITEM hItem,  BOOL isParent, DWORD param)
{
	ParamItem* pParamItem = (ParamItem*)pTreeCtrl->GetItemData(hItem);
	if(pParamItem)
	{
		pTreeCtrl->SetCheck(hItem, pParamItem->m_isChecked);
		pTreeCtrl->SetItemText(hItem, pParamItem->m_pName);

		ParamItem* pItem = NULL;
		if(pTreeCtrl->GetCheck(hItem))
		{
			pItem = (ParamItem*)pTreeCtrl->GetItemData(hItem);
			if(pItem)
			{
				pItem->m_isChecked = TRUE;
				if(pItem->m_Mask && pItem->m_pUint32)
				{
					Uint32Set(pItem->m_pUint32, pItem->m_Value, pItem->m_Mask);
					LOG((L"Set %s = 0x%08x\n", pItem->m_pName, *pItem->m_pUint32));
				}
			}
		}
		else
		{
			pItem = (ParamItem*)pTreeCtrl->GetItemData(hItem);
			if(pItem)
			{
				pItem->m_isChecked = FALSE;
				if(pItem->m_Mask && pItem->m_pUint32)
				{
					Uint32Set(pItem->m_pUint32, ~pItem->m_Value, pItem->m_Mask);
					LOG((L"Set %s = 0x%08x\n", pItem->m_pName, *pItem->m_pUint32));
				}
			}
		}	
		if(pItem->Checked && param == CCaseTree::CALL_CHECKED_FUN)
		{
			pItem->m_isAutoFlag = TRUE;
			pItem->Checked(pItem);
			pItem->m_isAutoFlag = FALSE;
		}
	}

	return FLAG_CONTINUE;
}

void CCaseTree::UpdateNode(ParamItem* pDeviceParams[],  int nDeviceParamsCount, WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hItem = GetNextItem(NULL,TVGN_ROOT);

	hItem = FindItem(hItem, (void*)wParam);

	if(NULL == hItem)
	{
		 hItem = GetNextItem(NULL,TVGN_ROOT);
	}
	SeatchItem(hItem, (OperateFun)Update, lParam);
}

void CCaseTree::Reset()
{

}

