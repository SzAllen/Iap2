// CaseTree.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "CaseTreeCtrl.h"

#
// CCaseTreeCtrl

IMPLEMENT_DYNAMIC(CCaseTreeCtrl, CMyTreeCtrl)

CCaseTreeCtrl::CCaseTreeCtrl()
{
	m_hSelectedItem = NULL;
	m_nCaseCount = 0;
	m_nSuccessCaseCount = 0;
	m_nFailedCaseCount = 0;
	m_nRunCaseCursor = 0;
	m_hCurrentItem = NULL;
	m_RunCaseThread = Null;
}

CCaseTreeCtrl::~CCaseTreeCtrl()
{
	SAFE_RELEASE_THREAD(m_RunCaseThread);
}

void CCaseTreeCtrl::Stop()
{
	SAFE_RELEASE_THREAD(m_RunCaseThread);
}

BEGIN_MESSAGE_MAP(CCaseTreeCtrl, CMyTreeCtrl)
	ON_COMMAND(ID_RUN, &CCaseTreeCtrl::OnRun)
	ON_COMMAND(ID_STOP, &CCaseTreeCtrl::OnStop)
END_MESSAGE_MAP()

BOOL CCaseTreeCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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

BOOL CCaseTreeCtrl::Init()
{
	return True;
}
// CCaseTreeCtrl 消息处理程序




void CCaseTreeCtrl::FillTreeNodeData(HTREEITEM pParent, TreeNodeData* pTreeNodeData[], int nCount, int& i, TreeNodeData* pParentTreeNodeData)
{
	HTREEITEM node = NULL;
	for(i = i; i < nCount; i++)
	{
		if(pParent)
		{
			//TreeNodeData_Dump(pTreeNodeData[i]);
			ASSERT(pParent && pParentTreeNodeData);
			if(pTreeNodeData[i]->m_Level == pParentTreeNodeData->m_Level + 1)
			{
				node = InsertItem(pTreeNodeData[i]->m_pName, 0, 0, pParent);
				SetItemData(node, (DWORD_PTR)pTreeNodeData[i]);
				i++;
				FillTreeNodeData(node, pTreeNodeData, nCount, i, pTreeNodeData[i-1]);
			}
			else
			{
				i--;
				return ;
			}
		}
		else if(pTreeNodeData[i]->m_Level == 0)
		{
			//TreeNodeData_Dump(pTreeNodeData[i]);
			node = InsertItem(pTreeNodeData[i]->m_pName, 0, 0);
			SetItemData(node, (DWORD_PTR)pTreeNodeData[i]);
			i++;
			FillTreeNodeData(node, pTreeNodeData, nCount, i, pTreeNodeData[i-1]);
		}
	}
}

void CCaseTreeCtrl::FillTreeNodeData(CCaseTreeData& testCase)
{
	int i = 0;
	FillTreeNodeData(NULL, testCase.m_pUnitTreeNodeDatas,  testCase.m_nUnitTreeNodeDatas, i, NULL);

	i = 0;
	FillTreeNodeData(NULL, testCase.m_pIntegTreeNodeDatas,  testCase.m_nIntegTreeNodeDatas, i, NULL);

	HTREEITEM hItem = GetNextItem(NULL,TVGN_ROOT);
	ExpendAll(hItem, True);
}


void CCaseTreeCtrl::OnContextMenu(CPoint point)
{
	HTREEITEM hTreeItem = NULL;

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		ScreenToClient(&ptTree);

		UINT flags = 0;
		hTreeItem = HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			SelectItem(hTreeItem);
		}
	}

	SetFocus();

	
	if(hTreeItem)
	{
		CMenu menu;
		/*
		if(wcscmp(pItem->m_pName, L"Emmc") == 0)
		{
			if (menu.LoadMenu(IDR_MENU2))
			{
				CMenu* pPopup = menu.GetSubMenu(0);
				ENSURE(pPopup != NULL);

				pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
									   point.x, point.y,
									   this); // route commands through main window,AfxGetMainWnd()
			}
		}
		else*/
		{
			if (menu.LoadMenu(IDR_MENU1))
			{
				CMenu* pPopup = menu.GetSubMenu(0);
				ENSURE(pPopup != NULL);

				pPopup->EnableMenuItem(ID_STOP, 0);
				pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
									   point.x, point.y,
									   this); // route commands through main window,AfxGetMainWnd()
			}
		}
	}	
}

void CCaseTreeCtrl::RunTestCase(HTREEITEM hItem, OperateFun pFn)
{
	SEARCH_FALG searchFlag = FLAG_CONTINUE;

	if (hItem != NULL)
	{
		searchFlag = pFn(this, hItem, 0, (DWORD)this);
		if(searchFlag == FLAG_CONTINUE)
		{
			hItem = GetNextItem(hItem, TVGN_CHILD);
			if (hItem != NULL)
			{
				searchFlag = SeatchItem(hItem,(OperateFun)pFn, (DWORD)this);
			}
		}
	}

	if(searchFlag == FLAG_CONTINUE)
	{
		m_hSelectedItem = NULL;
		m_hCurrentItem = NULL;
	}
}

static SEARCH_FALG RunTest(CCaseTreeCtrl* pTreeCtrl, HTREEITEM hTreeItem, BOOL isParent, DWORD param)
{
	TreeNodeData* pTreeNodeData = (TreeNodeData*)pTreeCtrl->GetItemData(hTreeItem);
	//CEmmcTesterDlg* pDlg = (CEmmcTesterDlg*)param;

	//TreeNodeData_Dump(pTreeNodeData);
	if(pTreeNodeData && pTreeNodeData->Run)
	{
		if(++pTreeCtrl->m_nCaseCount != pTreeCtrl->m_nRunCaseCursor) return FLAG_CONTINUE;

		//TreeNodeData_Dump(pTreeNodeData);
		Log(L"测试用例[%02d]：%s\n", pTreeCtrl->m_nRunCaseCursor, pTreeNodeData->m_pName);
		//if(pTreeNodeData->m_lpDesc)
		//	Log(L"\t%s\n", pTreeNodeData->m_lpDesc);
		pTreeCtrl->SetItemImage(hTreeItem, 3,3);
		Sleep(500);
		CHECK_STRING_RESET();
		BOOL isPass = pTreeNodeData->Run(pTreeNodeData);
		pTreeCtrl->m_nRunCaseCursor++;
		pTreeCtrl->m_hCurrentItem = hTreeItem;

		CTester::PrintTestResult(pTreeNodeData, isPass);
		if(isPass)
		{
			pTreeCtrl->m_nSuccessCaseCount++;
			pTreeCtrl->SetItemImage(hTreeItem, 1,1);
		}
		else
		{
			pTreeCtrl->m_nFailedCaseCount++;
			pTreeCtrl->SetItemImage(hTreeItem, 2, 2);
		}

		pTreeCtrl->Invalidate();
	}
	return FLAG_CONTINUE;
}

void CCaseTreeCtrl::RunTreeItem(DWORD param)
{
	SEARCH_FALG searchFlag = FLAG_CONTINUE;
	HTREEITEM hItem = NULL;
	if(m_hSelectedItem)
	{
		//MessageBox(_T("上一次运行没有结束，不能运行测试。");
		//return;
	}
	m_hSelectedItem = GetSelectedItem();
	TreeNodeData* pTreeNodeData = (TreeNodeData*)GetItemData(m_hSelectedItem);
	//if(pTreeNodeData)
	//{
		//pTreeNodeData->m_Param = param;
	//}

	m_nCaseCount = 0;
	m_nRunCaseCursor = 1;
	
	RunTestCase(m_hSelectedItem, (OperateFun)RunTest);

	Log(L"////////////////////////////////////////////////////\n测试结束\n");
	Log(L"测试用例总数：%d, 成功: %d, 失败: %d\n", m_nCaseCount, m_nSuccessCaseCount, m_nFailedCaseCount);
	Log(L"////////////////////////////////////////////////////\n");
}

void CCaseTreeCtrl::OnDbClick(HTREEITEM hItem)
{
	if(NULL == m_RunCaseThread)
	{
		OnRun();
	}
	else
	{
		OnStop();
	}
}

UINT RunCaseThread(CCaseTreeCtrl* pTreeCtrl)
{
	pTreeCtrl->m_hSelectedItem = pTreeCtrl->GetSelectedItem();

	CString strText;
	pTreeCtrl->GetWindowTextW(strText);

	pTreeCtrl->RunTreeItem(0);

	pTreeCtrl->m_RunCaseThread = NULL;
	return 0;
}

void CCaseTreeCtrl::OnRun()
{
	if(NULL == m_RunCaseThread)
	{
		m_nCaseCount = 0;
		m_nSuccessCaseCount = 0;
		m_nFailedCaseCount = 0;
		m_RunCaseThread	= (CWinThread*)AfxBeginThread((AFX_THREADPROC)RunCaseThread, this);
	}
	else
	{
		AfxMessageBox(L"测试用例正在运行中！");
	}
}

SEARCH_FALG CCaseTreeCtrl::StopTest(CCaseTreeCtrl* pTreeCtrl, HTREEITEM hTreeItem, BOOL isParent, DWORD param)
{
	TreeNodeData* pTreeNodeData = (TreeNodeData*)pTreeCtrl->GetItemData(hTreeItem);
	pTreeNodeData->m_isExit = TRUE;
	/*
	//TreeNodeData_Dump(pTreeNodeData);
	if(pTreeNodeData && pTreeNodeData->Stop)
	{
		if(++pTreeCtrl->m_nCaseCount != pTreeCtrl->m_nRunCaseCursor) return FLAG_CONTINUE;

		//CTester::PrintTreeNodeData(pTreeNodeData);
		pTreeNodeData->Stop(pTreeNodeData);
		pTreeCtrl->m_nRunCaseCursor++;
		pTreeCtrl->m_hCurrentItem = hTreeItem;

		Trace("停止测试\n");
		pTreeCtrl->SetItemImage(hTreeItem, 0, 0);
		pTreeCtrl->Invalidate();
	}
	*/
	return FLAG_CONTINUE;
}

void CCaseTreeCtrl::OnStop()
{
	// TODO: 在此添加命令处理程序代码

	SEARCH_FALG searchFlag = FLAG_CONTINUE;
	HTREEITEM hItem = NULL;

	m_hSelectedItem = GetSelectedItem();

	m_nCaseCount = 0;
	m_nSuccessCaseCount = 0;
	m_nFailedCaseCount = 0;
	m_nRunCaseCursor = 1;

	RunTestCase(m_hSelectedItem, (OperateFun)StopTest);
	Log(L"终止测试\n");


	//SAFE_RELEASE_THREAD(m_RunCaseThread);
	//Regester_ResetAll();
	//g_CVDevice.Reset();
	//m_SiteNumCtrl.SetCurSel(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
static SEARCH_FALG RunTestEx(CCaseTreeCtrl* pTreeCtrl, HTREEITEM hTreeItem, BOOL isParent, DWORD param)
{
	TreeNodeData* pTreeNodeData = (TreeNodeData*)pTreeCtrl->GetItemData(hTreeItem);
	//CEmmcTesterDlg* pDlg = (CEmmcTesterDlg*)param;

	//TreeNodeData_Dump(pTreeNodeData);
	if(pTreeNodeData && pTreeNodeData->Run)
	{
		if(++pTreeCtrl->m_nCaseCount != pTreeCtrl->m_nRunCaseCursor) return FLAG_CONTINUE;

		pTreeCtrl->SetItemImage(hTreeItem, 3,3);
		Sleep(500);
		CHECK_STRING_RESET();
		BOOL isPass = pTreeNodeData->Run(pTreeNodeData);
		pTreeCtrl->m_nRunCaseCursor++;
		pTreeCtrl->m_hCurrentItem = hTreeItem;

		CTester::PrintTestResult(pTreeNodeData, isPass);
		if(isPass)
		{
			pTreeCtrl->m_nSuccessCaseCount++;
			pTreeCtrl->SetItemImage(hTreeItem, 1,1);
		}
		else
		{
			pTreeCtrl->m_nFailedCaseCount++;
			pTreeCtrl->SetItemImage(hTreeItem, 2, 2);
		}

		pTreeCtrl->Invalidate();
	}
	return FLAG_CONTINUE;
}
void CCaseTreeCtrlEx::RunTreeItem(DWORD param)
{
	SEARCH_FALG searchFlag = FLAG_CONTINUE;
	HTREEITEM hItem = NULL;
	if(m_hSelectedItem)
	{
		//MessageBox(_T("上一次运行没有结束，不能运行测试。");
		//return;
	}
	m_hSelectedItem = GetSelectedItem();
	TreeNodeData* pTreeNodeData = (TreeNodeData*)GetItemData(m_hSelectedItem);
	//if(pTreeNodeData)
	//{
		//pTreeNodeData->m_Param = param;
	//}

	m_nCaseCount = 0;
	m_nRunCaseCursor = 1;
	
	RunTestCase(m_hSelectedItem, (OperateFun)RunTestEx);
}
