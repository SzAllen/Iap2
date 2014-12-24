#pragma once

#include "TestCase.h"
#include "DeviceTree.h"

typedef enum _SEARCH_FALG
{
	 FLAG_BREAK = 0
	,FLAG_CONTINUE
}SEARCH_FALG;
typedef SEARCH_FALG (*OperateFun)(void* pTreeCtrl, HTREEITEM hItem,  BOOL isParent, DWORD param);


// CCaseTree
class CCaseTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(CCaseTree)

protected:
public:
	void Reset();
	SEARCH_FALG SeatchItem(HTREEITEM hItem, OperateFun Operate, DWORD param);
	HTREEITEM FindItem(HTREEITEM hItem, void* pItemData);
	void FillTestItem();
	void FillDeviceItem();
	void UpdateNode(ParamItem* pDeviceParams[],  int nDeviceParamsCount, WPARAM wParam, LPARAM lParam);
	BOOL Init();
public:
	enum
	{
		CALL_CHECKED_FUN = 0x01
	};
	CCaseTree();
	virtual ~CCaseTree();

protected:
	void ExpendAll(HTREEITEM hItem, BOOL isExpend);
	void FillTestItem(HTREEITEM pParent, TestItem* pTestItem[], int nCount, int& nIndex, TestItem* pParentTestItem);
	void FillParamItem(HTREEITEM pParent, ParamItem* pParamItem[], int nCount, int& i, ParamItem* pParentParamItem);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
protected:
	DECLARE_MESSAGE_MAP()
};


