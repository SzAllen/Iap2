#pragma once

#include "MainDlg.h"
#include "XTabCtrl.h"
#include "afxcmn.h"
#include "CaseTreeCtrl.h"

// CDlgIap �Ի���

class CDlgIap : public CTabDlg
{
	DECLARE_DYNAMIC(CDlgIap)

public:
	CCaseTreeCtrl m_CasesTreeCtrl;
	CCaseTreeCtrlEx m_CasesTreeCtrl2;
	CImageList m_FileViewImages;

public:
	CDlgIap(CLog *pLog, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgIap();

// �Ի�������
	enum { IDD = IDD_DLG_IAP2_TESTER };

protected:
	HTREEITEM GetSelectedItem(CTreeCtrl* pWndTree, UINT& flags);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclkTreeTesecase(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTreeTesecase2(NMHDR *pNMHDR, LRESULT *pResult);
};
