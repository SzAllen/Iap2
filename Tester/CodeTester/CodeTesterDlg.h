
// CodeTesterDlg.h : 头文件
//

#pragma once

#include "CaseTreeCtrl.h"
#include "LogCtrl.h"
#include "afxwin.h"

// CCodeTesterDlg 对话框
class CCodeTesterDlg : public CDialogEx
{
// 构造
public:
	//CTesterCm* m_pTesterCm;
	CCodeTesterDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BMCTESTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnNMMDblclkCaseTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickHwTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickCmTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult);

	/*
	afx_msg void OnRunTest();
	afx_msg void OnSetUp();
	afx_msg LRESULT OnCaseDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateDeviceTree(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedClose();
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStop();
	afx_msg void OnWatchDogReset();
	afx_msg void OnTvnItemChangedTree2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeSitenumber();
	afx_msg void OnRunNormal();
	afx_msg void OnBnClickedInput();
	*/
	DECLARE_MESSAGE_MAP()
protected:
	HTREEITEM m_hSelectedItem;
	CImageList m_FileViewImages;

	void RunTestCase(HTREEITEM hItem, OperateFun pFun);
	void OnRunTest(DWORD param);
	HTREEITEM GetSelectedItem(CTreeCtrl* pWndTree, UINT& flags);
public:
	void OutPut(LPCTSTR lpStr);
	void RunCaseDone(HTREEITEM hTreeItem, BOOL isPass);
	void SiteNumberComboBoxInit();
	static BYTE SetLed1State(BYTE isOn);
	static BYTE SetRunLedState(BYTE isOn);
	static BYTE SetBlueLedState(BYTE isOn);
	void SendUdp(BYTE* pData, int len);
	void OnSave();
	BOOL isExist(const CString& str);
public:
	CCaseTreeCtrl m_TestCaseTreeCtrl;
	CLogCtrl m_OutPutCtrl;
	HTREEITEM m_hCurrentItem;
	int m_nCaseCount;
	int m_nRunCaseCursor;
	CButton m_RadioRunLedCtrl;
	CButton m_RadioLed1Ctrl;
	CButton m_RadioBlueLedCtrl;
	CEdit m_InputEditCtrl;
	afx_msg void OnBnClickedInput();
	afx_msg void OnBnClickedReset();
	afx_msg void OnClose();
	CComboBox m_CmbInput;
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnCbnSelchangeComboInput();
};

UINT ReceiveData(LPVOID pParam);