#ifndef  _PRINTF_H_
#define  _PRINTF_H_

#ifdef __cplusplus
extern "C"{
#endif
#include "ArchDef.h"

#define MAX_PFBUFF_SIZE 256
typedef int (*PrintfFun)(const char* pszFormat, ...);

extern PrintfFun g_printFn;
#define Printf g_printFn

extern char g_Pfbuffer[MAX_PFBUFF_SIZE];

int __Printf( const char* ctrl1, ...);

int vsnprintfEx(char* buffer, int bufLen, const char* lpszFormat, va_list ptr);

#if CONFIG_PFSOCKET
void Pf(unsigned char isToPfSocket);
#else
#define Pf(...);
#endif

#ifdef _WINDOWS
extern PrintfFun TraceEx;
#endif

#ifdef __cplusplus
}
#endif

#endif             

