#pragma once


// CDlgMoudleNum 对话框
#include "XTabCtrl.h"
#include "EditFileName.h"

class CDlgMoudleNum : public CTabDlg
{
	DECLARE_DYNAMIC(CDlgMoudleNum)

public:
	CEditFileName	m_CtrlLogFile;
	CButton			m_BtnDownLoad;

	CComboBox		m_ComboColor;
	CComboBox		m_ComboCapa;
	CComboBox		m_ComboProduct;
public:
	void SetComboCapa(BYTE capaAllignG);
public:
	CDlgMoudleNum(CLog *pLog, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMoudleNum();

// 对话框数据
	enum { IDD = IDD_DLG_MOUDLE_NUM };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnNotify(EVENT eEvent, WPARAM param, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedBtnLogfile();
};
