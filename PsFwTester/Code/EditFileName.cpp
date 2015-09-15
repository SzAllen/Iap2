// EditFileName.cpp : 实现文件
//

#include "stdafx.h"
#include "MainApp.h"
#include "EditFileName.h"

// CEditFileName
IMPLEMENT_DYNAMIC(CEditProfile, CEdit)

CEditProfile::CEditProfile()
{

}

CEditProfile::~CEditProfile()
{
}

BEGIN_MESSAGE_MAP(CEditProfile, CEdit)
END_MESSAGE_MAP()

void CEditProfile::Init(CProfile* pProfile, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileNameFilter)
{
	CProFileRecord::Init(pProfile, lpAppName, lpKeyName, lpDefault, lpFileNameFilter);
	SetWindowText(m_Value);
}

void CEditProfile::Reflash()
{
	CProFileRecord::Reflash();
	SetWindowText(m_Value);
}

BOOL CEditProfile::Save()
{
	GetWindowText(m_Value);
	return CProFileRecord::Save();
}

// CEditFileName
//////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CEditFileName, CEditProfile)

CEditFileName::CEditFileName()
{

}

CEditFileName::~CEditFileName()
{
}


BEGIN_MESSAGE_MAP(CEditFileName, CEditProfile)
END_MESSAGE_MAP()

BOOL CEditFileName::OpenFileDlg(LPCTSTR lpszFilter)
{
	//static char szFilter[] = _T("Binary File (*.bin)|*.bin|");
	CString szCodeFile;
	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		lpszFilter);

	if(dlg.DoModal() == IDOK)
	{
		szCodeFile = dlg.GetPathName();
		SetWindowText(szCodeFile);
		Save();
		return TRUE;
	}

	return FALSE;
}

BOOL CEditFileName::OpenFolderDlg(LPCTSTR lpszFolder)
{
	//static char szFilter[] = _T("Binary File (*.bin)|*.bin|");
	CString szCodeFile;
	CFolderPickerDialog dlg(lpszFolder);

	if(dlg.DoModal() == IDOK)
	{
		szCodeFile = dlg.GetPathName();
		SetWindowText(szCodeFile);
		Save();
		return TRUE;
	}

	return FALSE;
}

// CEditFileName 消息处理程序


//////////////////////////////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CEditNum, CEditProfile)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////

void CComboProfile::Init(CProfile* pProfile, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileNameFilter)
{
	CProFileRecord::Init(pProfile, lpAppName, lpKeyName, lpDefault, lpFileNameFilter);
	for(int i = 0; i < this->GetCount(); i++)
	{
		CString str;
		GetLBText(i, str);
		if(str == m_Value)
		{
			SetCurSel(i);
			return;
		}
	}

	SetCurSel(0);
}

void CComboProfile::Reflash()
{
	CProFileRecord::Reflash();
	SetWindowText(m_Value);
}

BOOL CComboProfile::Save()
{
	GetWindowText(m_Value);
	return CProFileRecord::Save();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void CCheckBox::Init(CProfile* pProfile, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileNameFilter)
{
	CProFileRecord::Init(pProfile, lpAppName, lpKeyName, lpDefault, lpFileNameFilter);
	
	SetCheck(m_Value != L"0");
}

void CCheckBox::Reflash()
{
	CProFileRecord::Reflash();
	SetWindowText(m_Value);
}

BOOL CCheckBox::Save()
{
	m_Value = GetCheck() ? L"1" : L"0";

	return CProFileRecord::Save();
}