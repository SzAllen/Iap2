#include "StdAfx.h"
#include "LogCtrl.h"

CLogCtrl::CLogCtrl(void)
{
	//m_String.Preallocate(1024*1024);
}


CLogCtrl::~CLogCtrl(void)
{
}

void CLogCtrl::WriteData(LPCTSTR lpStr)
{
	SetSel(GetWindowTextLengthW(),GetWindowTextLengthW());
	ReplaceSel(lpStr);
}

void CLogCtrl::WriteData(const char* lpStr)
{
	SetSel(GetWindowTextLengthW(),GetWindowTextLengthW());
	CString str(lpStr);
	ReplaceSel(str);
}

void CLogCtrl::Log(const BYTE* lpStr, int len)
{
#define MAX_LEN 1024
	CHAR ch[MAX_LEN]={0};

	ASSERT(len <= MAX_LEN/2);

	for(int i = 0; i < len; i++)
	{
		sprintf_s(&ch[i*2], MAX_LEN, "%02x", lpStr[i]);
	}

	WriteData(ch);
}

int CLogCtrl::Log(LPSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	char pszBuffer[1024];
#pragma warning (push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)
	vsprintf( pszBuffer, pszFormat, ptr );
#pragma warning (pop)
	va_end(ptr);

	WriteData(pszBuffer);

	return 0;
}


int CLogCtrl::Log(LPCTSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	wchar_t pszBuffer[1024];
#pragma warning (push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)
	vswprintf( pszBuffer, pszFormat, ptr );
#pragma warning (pop)
	va_end(ptr);

	WriteData(pszBuffer);

	return 0;
}


void CLogCtrl::Reset()
{
	m_String = _T("");
	SetWindowTextW(L"");
}

