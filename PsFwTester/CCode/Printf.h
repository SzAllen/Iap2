#ifndef  _PRINTF_H_
#define  _PRINTF_H_

#ifdef __cplusplus
extern "C"{
#endif

#ifdef XDEBUG

#include "ArchDef.h"
#include <stdarg.h>
  
#define MAX_PFBUFF_SIZE 256
typedef int (*PrintfFun)(const char* pszFormat, ...);

extern PrintfFun g_printFn;
#define Printf g_printFn
//#define Printf(...)

extern char g_Pfbuffer[MAX_PFBUFF_SIZE];

int __Printf( const char* ctrl1, ...);
int Printf_None(const char* lpszFormat, ...);

int vsnprintfEx(char* buffer, int bufLen, const char* lpszFormat, va_list ptr);

#ifdef _WINDOWS
extern PrintfFun TraceEx;
#endif

#endif	//#ifdef XDEBUG

#ifdef __cplusplus
}
#endif

#endif             

