#pragma once
class CLog
{
public:
	CLog(void);
	~CLog(void);
	virtual void Reset(){};
	virtual void Log(LPCTSTR pszFormat, ...) = 0;
	virtual void Log(const char* pszFormat, ...) = 0;
	void DumpByte(const BYTE* pData, short len, BYTE cols = 32);
};

