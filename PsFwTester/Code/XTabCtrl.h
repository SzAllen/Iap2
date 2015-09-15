#if !defined(AFX_XTABCTRL_H__A11951B3_2F95_11D3_A896_00A0C9B6FB28__INCLUDED_)
#define AFX_XTABCTRL_H__A11951B3_2F95_11D3_A896_00A0C9B6FB28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXTabCtrl window
#include <afxtempl.h>
#include "Serial.h"

#define WM_ITEM_SELECTED			(WM_USER + 100)

class CXTabCtrl : public CTabCtrl
{
// Construction
public:
	CXTabCtrl();

// Attributes
public:

// Operations
	class CTabDlg* GetTableItemEx(int index);
	void AddTabDlg(class CTabDlg* pTabDl, LPCTSTR lpszCaption, int iImage =0);
public:
	void AddTab(CWnd* pWnd, LPCTSTR lpszCaption, int iImage =0);
	void EnableTab(int iIndex, BOOL bEnable = TRUE);
	BOOL SelectTab(int iIndex);
	void DeleteAllTabs();
	void DeleteTab(int iIndex);
	void SetTopLeftCorner(CPoint pt);
	BOOL IsTabEnabled(int iIndex);

	void SetDisabledColor(COLORREF cr);
	void SetSelectedColor(COLORREF cr);
	void SetNormalColor(COLORREF cr);
	void SetMouseOverColor(COLORREF cr);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTabCtrl)
	protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SelectNextTab(BOOL bForward);
	BOOL SelectTabByName(LPCTSTR lpszTabCaption);
	void ChangeTab(int iIndex, CWnd* pNewTab, LPTSTR lpszCaption, int iImage);
	virtual ~CXTabCtrl();

	// Generated message map functions
protected:
	CArray<BOOL, BOOL> m_arrayStatusTab; //** enabled Y\N
	int m_iSelectedTab;
	POINT m_ptTabs;
	COLORREF m_crSelected;
	COLORREF m_crDisabled;
	COLORREF m_crNormal;
	COLORREF m_crMouseOver;

	int m_iIndexMouseOver;
	
	bool m_bMouseOver;
	bool m_bColorMouseOver;
	bool m_bColorNormal;
	bool m_bColorDisabled;
	bool m_bColorSelected;
	
	//{{AFX_MSG(CXTabCtrl)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/*
class CTabCtrlEx : public CXTabCtrl
{
public:
	CTabCtrlEx();
	virtual ~CTabCtrlEx();
public:
protected:
	DECLARE_MESSAGE_MAP()
};
*/
/////////////////////////////////////////////////////////////////////////////
#define LOG m_pLog->Log

typedef enum _EVENT
{
	EVENT_USB_PLUG
	,EVENT_DOWNLOAD_PROCESS
}EVENT;

class CTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTabDlg)
public:
	UINT		m_DlgID;
	CXTabCtrl*	m_pTab;
	CLog*		m_pLog;
public:
	CTabDlg(CLog *pLog, DWORD dlgID, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTabDlg();
//	void Config(CUDisk*	pDisk){m_pDisk = pDisk;};

	virtual BOOL Create(UINT nIDTemplate, CXTabCtrl* pTab);
	virtual void OnNotify(EVENT eEvent, WPARAM param = 0, LPARAM lparam = 0){};
	virtual void SelChanged(CTabDlg* pNewTabDlg){};
protected:
	afx_msg LRESULT OnSelchange(WPARAM wparam, LPARAM lp);
//	afx_msg LRESULT OnOk(WPARAM wparam, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XTABCTRL_H__A11951B3_2F95_11D3_A896_00A0C9B6FB28__INCLUDED_)
