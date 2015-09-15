#pragma once

#include "Profile.h"
// CEditFileName

class CEditProfile : public CEdit, public CProFileRecord
{
	DECLARE_DYNAMIC(CEditProfile)

protected:

	CProfile* m_pProfile;

public:
	CEditProfile();
	virtual ~CEditProfile();
	
	virtual CString GetString(){CString str; GetWindowText(str); return str;};
	virtual void Init(CProfile* pProfile, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileNameFilter = NULL);
	virtual BOOL Save();
	virtual void Reflash();

protected:
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
class CEditFileName : public CEditProfile
{
	DECLARE_DYNAMIC(CEditFileName)
protected:
public:
	CEditFileName();
	virtual ~CEditFileName();

	BOOL OpenFileDlg(LPCTSTR lpszFilter = NULL);
	BOOL OpenFolderDlg(LPCTSTR lpszFolder = NULL);
protected:
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
class CEditNum : public CEditProfile
{
protected:
	int m_nMin;
	int m_nMax;
public:
	CEditNum(){m_nMin = 0; m_nMax = 0;};
	virtual ~CEditNum(){};

	void SetDwRange(DWORD dwMin, DWORD dwMax){m_nMin = dwMin; m_nMax = dwMax;};
	int GetValue(){char str[32]={0}; GetWindowTextA(m_hWnd, str, 32); return atoi(str);};
	void SetValue(int Value, LPCTSTR lpFormat=L"%d"){CString str;str.Format(lpFormat,Value) ;SetWindowText(str);};
protected:
	DECLARE_MESSAGE_MAP()
};

// CEditFileName

class CComboProfile : public CComboBox, public CProFileRecord
{

protected:

	CProfile* m_pProfile;

public:
	CComboProfile(){};
	virtual ~CComboProfile(){};
	
	CString GetString(){CString str; GetWindowText(str); return str;};
	virtual void Init(CProfile* pProfile, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileNameFilter = NULL);
	virtual BOOL Save();
	virtual void Reflash();

protected:
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
class CCheckBox : public CButton, public CProFileRecord
{

protected:
	CProfile* m_pProfile;

public:
	CCheckBox(){};
	virtual ~CCheckBox(){};
	
	CString GetString(){CString str; GetWindowText(str); return str;};
	virtual void Init(CProfile* pProfile, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileNameFilter = NULL);
	virtual BOOL Save();
	virtual void Reflash();

protected:
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
