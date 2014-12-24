#ifndef __ARCH_CFG_H_
#define __ARCH_CFG_H_

#ifdef __cplusplus
extern "C"{
#endif

#define MAX_MSG_COUNT	60	//Message queue deepth
#define MAX_TIMER_COUNT	16
#define MS_STICKS 		16	//Hot many ticks per ms, it is for timer
#define TIMER_TASK_INTERVAL 1

#include "typedef.h"	
#include <String.h>
#include <stdio.h>

#include "Debug.h"	

#ifdef _WINDOWS

	#define SLEEP 		Sleep
	#define LOCK()		{if(g_CriticalSection.OwningThread) EnterCriticalSection(&g_CriticalSection)
	#define UNLOCK()	if(g_CriticalSection.OwningThread) LeaveCriticalSection(&g_CriticalSection);}
	#define GET_TICKS() GetTickCount()
	
	extern CRITICAL_SECTION g_CriticalSection;
#else
	#define SLEEP(...)
	#define LOCK(...)
	#define UNLOCK(...)
	#define GET_TICKS(...)
#endif

int Osa_Init();
#ifdef __cplusplus
}
#endif

#endif




