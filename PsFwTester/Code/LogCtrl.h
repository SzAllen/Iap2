#pragma once
#include "afxcmn.h"
class CLogCtrl : public CRichEditCtrl
{
public:
	CLogCtrl(void);
	~CLogCtrl(void);
protected:
	CString m_String;
	void WriteData(LPCTSTR lpStr);
	void WriteData(const char* lpStr);

public:
	void Log(const BYTE* lpStr, int len);
	int Log(LPSTR pszFormat, ...);
	int Log(LPCTSTR pszFormat, ...);

	void Reset();
};

