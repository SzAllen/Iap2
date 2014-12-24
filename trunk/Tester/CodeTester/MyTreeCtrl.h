#pragma once

typedef enum _SEARCH_FALG
{
	 FLAG_BREAK = 0
	,FLAG_CONTINUE
}SEARCH_FALG;
typedef SEARCH_FALG (*OperateFun)(void* pTreeCtrl, HTREEITEM hItem,  BOOL isParent, DWORD param);

// CMyTreeCtrl

class CMyTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CMyTreeCtrl)

public:
	CMyTreeCtrl();
	virtual ~CMyTreeCtrl();
public:
	SEARCH_FALG SeatchItem(HTREEITEM hItem, OperateFun Operate, DWORD param);
	HTREEITEM FindItem(HTREEITEM hItem, void* pItemData);
	void ExpendAll(HTREEITEM hItem, BOOL isExpend);

protected:
	DECLARE_MESSAGE_MAP()
};


