#pragma once
class CProfile
{
	CWinApp*	m_pApp;
	CStringW m_ProfileName;
	CStringW m_Path;
public:
	CProfile();
	~CProfile(void);
public:
	void Init(CWinApp& m_App);

	BOOL WriteStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR value);
	BOOL WriteInt(LPCWSTR lpAppName, LPCWSTR lpKeyName, int value);
	BOOL WriteStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, CString& strRet);
	UINT ReadInt(LPCWSTR lpAppName, LPCWSTR lpKeyName, UINT nDefault);
	DWORD ReadStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, WCHAR lpRetStr[], int size);
	DWORD ReadStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, CStringA& strRet);
	DWORD ReadStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, CString& strRet);
	DWORD ReadStr(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, CStringA& strRet);
	CHAR* ReadStr(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault);
};

class CProFileRecord
{
protected:
	CString m_AppName;
	CString m_KeyName;
	CString m_Default;

	CString m_Value;

	CProfile* m_pProfile;

public:
	CProFileRecord(){};
	virtual ~CProFileRecord(){};
	
	CString FindFile(LPCTSTR lpFilter, LPCTSTR lpPath = L".\\");
	virtual CString GetString();
	virtual void Init(CProfile* pProfile, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileNameFilter = NULL);
	virtual BOOL Save();
	virtual void Reflash();
	virtual BOOL Check(){return TRUE;};

};

extern CProfile g_Profile;
