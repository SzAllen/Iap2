#include "stdafx.h"
#include "Profile.h"

CProfile g_Profile;

CProfile::CProfile()
{
}


CProfile::~CProfile(void)
{
}

void CProfile::Init(CWinApp& app)
{	m_pApp = &app;
	WCHAR buff[100];
	GetCurrentDirectoryW(100, buff);
	m_ProfileName = buff;
	m_ProfileName += "\\";
	m_Path = m_ProfileName;
	m_ProfileName += app.m_pszProfileName;

	WIN32_FIND_DATAW findFileData;
	HANDLE handle = FindFirstFileW(m_ProfileName, &findFileData);
	if(0xFFFFFFFF ==  (UINT)handle)
	{
	}
}

BOOL CProfile::WriteStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR value)
{
	return ::WritePrivateProfileStringW(lpAppName, lpKeyName, value, m_ProfileName);
}

BOOL CProfile::WriteStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, CString& str)
{
	CStringW strw(str);
	return ::WritePrivateProfileStringW(lpAppName, lpKeyName, strw.LockBuffer(), m_ProfileName);
}

BOOL CProfile::WriteInt(LPCWSTR lpAppName, LPCWSTR lpKeyName, int value)
{
	CStringW str;
	str.Format(L"%d",value);
	return ::WritePrivateProfileStringW(lpAppName, lpKeyName, str.LockBuffer(), m_ProfileName);
}

UINT CProfile::ReadInt(LPCWSTR lpAppName, LPCWSTR lpKeyName, UINT nDefault)
{
	return ::GetPrivateProfileIntW(lpAppName, lpKeyName,  nDefault, m_ProfileName);
}

DWORD CProfile::ReadStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, WCHAR lpRetStr[], int size)
{
	return ::GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, lpRetStr, size, m_ProfileName);
}

DWORD CProfile::ReadStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, CString& strRet)
{
	DWORD dwRet = 0;
	const static int LEN = 1024;
	WCHAR buff[LEN]={0};

	dwRet = ::GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, buff, LEN, m_ProfileName);
	strRet = buff;
	return dwRet;
}

DWORD CProfile::ReadStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, CStringA& strRet)
{
	DWORD dwRet = 0;
	const static int LEN = 100;
	WCHAR buff[LEN]={0};

	dwRet = ::GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, buff, LEN, m_ProfileName);
	strRet = buff;

	return dwRet;
}
DWORD CProfile::ReadStr(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, CStringA& strRet)
{
	CStringA fileName(m_ProfileName);
	DWORD dwRet = 0;
	const static int LEN = 256;
	CHAR buff[LEN]={0};

	dwRet = ::GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, buff, LEN, fileName);
	strRet = buff;

	return dwRet;
}
CHAR* CProfile::ReadStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault)
{
	static CStringA strRet;
	ReadStr(lpAppName, lpKeyName, lpDefault, strRet);

	return strRet.LockBuffer();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

CString CProFileRecord::FindFile(LPCTSTR lpFilter, LPCTSTR lpPath)
{
	CFileFind finder;
	CString strFileName;

	CString fullFileName = L"";
	fullFileName.Format(L"%s%s", lpPath, lpFilter);
	BOOL bWorking = finder.FindFile(fullFileName);

	if(bWorking)
	{
	   bWorking = finder.FindNextFile();

	   strFileName = finder.GetFileName();
	   TRACE(_T("%s\n"), (LPCTSTR)finder.GetFileName());
	   strFileName.Format(L"%s%s", lpPath, finder.GetFileName());
		return strFileName;
	}
	return L"";
}

void CProFileRecord::Init(CProfile* pProfile, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileNameFilter)
{
	CString str;

	m_pProfile = pProfile;
	m_AppName = lpAppName;
	m_KeyName = lpKeyName;
	m_Default = lpDefault;

	pProfile->ReadStr(lpAppName, lpKeyName, m_Default, str);

	if(str == L"" && lpFileNameFilter != L"")
	{
		str = FindFile(lpFileNameFilter);
	}

	m_Value = str;
}

void CProFileRecord::Reflash()
{
	m_pProfile->ReadStr(m_AppName, m_KeyName, m_Default, m_Value);
}

BOOL CProFileRecord::Save()
{
	if(Check())
	{
		return m_pProfile->WriteStr(m_AppName, m_KeyName, m_Value);
	}

	return FALSE;
}

CString CProFileRecord::GetString()
{
	return L"";
};
