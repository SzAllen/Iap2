// DlgIap.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgIap.h"
#include "afxdialogex.h"
#include "MainApp.h"

// CDlgIap �Ի���

IMPLEMENT_DYNAMIC(CDlgIap, CTabDlg)

CDlgIap::CDlgIap(CLog *pLog, CWnd* pParent /*=NULL*/)
	: CTabDlg(pLog, CDlgIap::IDD, pParent)
{

}

CDlgIap::~CDlgIap()
{
}

void CDlgIap::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TESECASE, m_CasesTreeCtrl);
	DDX_Control(pDX, IDC_TREE_TESECASE2, m_CasesTreeCtrl2);
}


BEGIN_MESSAGE_MAP(CDlgIap, CTabDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_TESECASE, &CDlgIap::OnNMDblclkTreeTesecase)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_TESECASE2, &CDlgIap::OnNMDblclkTreeTesecase2)
END_MESSAGE_MAP()


// CDlgIap ��Ϣ�������
BOOL CDlgIap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_CasesTreeCtrl.Init();
	m_FileViewImages.Create(IDB_BITMAP2, 16, 0, RGB(255, 255, 255));
	m_CasesTreeCtrl.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
	m_CasesTreeCtrl.FillTreeNodeData(*GetTreeData());

	m_CasesTreeCtrl2.Init();
	m_CasesTreeCtrl2.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
	m_CasesTreeCtrl2.FillTreeNodeData(*GetInputTreeData());
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

HTREEITEM CDlgIap::GetSelectedItem(CTreeCtrl* pWndTree, UINT& flags)
{
	HTREEITEM hItem = NULL;
	//TRACE(L"%s()\n", _FUNC_);

	ASSERT_VALID(pWndTree);
	DWORD dwPos = GetMessagePos();
	CPoint point(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
	
	if (point != CPoint(-1, -1))
	{
		// ѡ���ѵ�������:
		pWndTree->ScreenToClient(&point);

		hItem = pWndTree->HitTest(point, &flags);

		//TRACE(L"%s() flags=%d\n", _FUNC_, flags);
		if(flags & TVHT_ONITEMLABEL || flags & TVHT_ONITEMSTATEICON)
		{
			if (hItem != NULL)
			{
				pWndTree->SelectItem(hItem);
			}
		}
		else
		{
			return NULL;
		}
	}

	return hItem;
}

void CDlgIap::OnNMDblclkTreeTesecase(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	UINT flags = 0;
	HTREEITEM hItem = GetSelectedItem(&m_CasesTreeCtrl, flags);

	if(hItem)
	{
		m_CasesTreeCtrl.OnDbClick(hItem);
	}
	//MsgBox(L"Test");
}

void CDlgIap::OnNMDblclkTreeTesecase2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	UINT flags = 0;
	HTREEITEM hItem = GetSelectedItem(&m_CasesTreeCtrl2, flags);

	if(hItem)
	{
		m_CasesTreeCtrl2.OnDbClick(hItem);
	}
	//MsgBox(L"Test");
}
