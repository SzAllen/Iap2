#pragma once

#include "CaseTreeData.h"
#include "MyTreeCtrl.h"

// CCaseTreeCtrl
class CCaseTreeCtrl : public CMyTreeCtrl
{
	DECLARE_DYNAMIC(CCaseTreeCtrl)

public:
	HTREEITEM m_hSelectedItem;
	int m_nCaseCount;
	int m_nSuccessCaseCount;
	int m_nFailedCaseCount;
	int m_nRunCaseCursor;
	HTREEITEM m_hCurrentItem;
public:
	void FillTreeNodeData(CCaseTreeData& testCase);
	BOOL Init();
	void Stop();
	void OnContextMenu(CPoint point);
	void OnDbClick(HTREEITEM hItem);
public:
	CCaseTreeCtrl();
	virtual ~CCaseTreeCtrl();

	CWinThread*	m_RunCaseThread;

	virtual void RunTreeItem(DWORD param);

protected:
	void FillTreeNodeData(HTREEITEM pParent, TreeNodeData* pTreeNodeData[], int nCount, int& nIndex, TreeNodeData* pParentTreeNodeData);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void RunTestCase(HTREEITEM hItem, OperateFun pFn);
	static SEARCH_FALG StopTest(CCaseTreeCtrl* pTreeCtrl, HTREEITEM hTreeItem, BOOL isParent, DWORD param);
protected:
	afx_msg void OnRun();
	afx_msg void OnStop();
	DECLARE_MESSAGE_MAP()
};

class CCaseTreeCtrlEx : public CCaseTreeCtrl
{
public:
	CCaseTreeCtrlEx(){};
	virtual ~CCaseTreeCtrlEx(){};

	virtual void RunTreeItem(DWORD param);
};
