
#include "ArchDef.h"
#include "Printf.h"

PrintfFun g_printFn = __Printf;
#ifdef _WINDOWS
PrintfFun TraceEx = Null;
#endif

int vsnprintfEx(char* buffer, int bufLen, const char* lpszFormat, va_list ptr)
{
	if(DL_SHOW_TICKS & g_dwDebugLevel)
	{
		int i = 0;
		int nLen = 0;
		static Bool isNewLine = True;
		
		if(isNewLine)
		{
			if('\n' == lpszFormat[0])
			{
				buffer[0] = '\n';
				i = 1;
			}
			nLen = sprintf(&buffer[i], "[%08x]:", GET_TICKS()) + i;
		}
		
		if(strlen(lpszFormat) >= 1)
		{
			isNewLine = (lpszFormat[strlen(lpszFormat) - 1] == '\n');
		}
		
		nLen = vsnprintf(&buffer[nLen], bufLen - nLen, &lpszFormat[i], ptr) + nLen;

		return nLen;
	}
	else
	{
		return vsnprintf(buffer, bufLen, lpszFormat, ptr);
	}
}

char g_Pfbuffer[MAX_PFBUFF_SIZE];
int __Printf(const char* lpszFormat, ...)
{
	int nLen = 0;
	va_list ptr;

	LOCK();

	memset(g_Pfbuffer, 0, sizeof(g_Pfbuffer));
	va_start(ptr, lpszFormat);
	nLen = vsnprintfEx(g_Pfbuffer, sizeof(g_Pfbuffer), lpszFormat, ptr);
	va_end(ptr);
	
	#ifdef _WINDOWS
	if(TraceEx) TraceEx(g_Pfbuffer);
	#endif

	UNLOCK();

	return nLen;
}



