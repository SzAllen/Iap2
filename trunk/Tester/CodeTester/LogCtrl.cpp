#include "StdAfx.h"
#include "LogCtrl.h"

CLogCtrl::CLogCtrl(void)
{
	//m_String.Preallocate(1024*1024);
}


CLogCtrl::~CLogCtrl(void)
{
}

void CLogCtrl::OutPut(LPCTSTR lpStr)
{
	SetSel(GetWindowTextLengthW(),GetWindowTextLengthW());
	ReplaceSel(lpStr);
}

void CLogCtrl::OutPut(const char* lpStr, int len)
{
	CString str(lpStr, len);

	SetSel(GetWindowTextLengthW(),GetWindowTextLengthW());
	ReplaceSel(str);
}

void CLogCtrl::Reset()
{
	m_String = _T("");
	SetWindowTextW(L"");
}

