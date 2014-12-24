#pragma once
#include "afxcmn.h"
class CLogCtrl :
	public CRichEditCtrl
{
public:
	CLogCtrl(void);
	~CLogCtrl(void);
protected:
	CString m_String;
public:
	void OutPut(LPCTSTR lpStr);
	void OutPut(const char* lpStr, int len);
	void Reset();
};

