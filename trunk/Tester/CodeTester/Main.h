/******************************************************************************
Main.h 
`
����:
1. ʵ����Ŀ��ں�������

����:
������   2013.09.3    �����ļ�.
******************************************************************************/

#ifndef  __MAIN_H__
#define  __MAIN_H__

#ifdef _cplusplus
extern "C"{
#endif

#include "Global.h"
#include "WirelessGate.h"

extern struct _WirelessGate*	g_pWg;

void Main(void);

//uint32 SLEEP(uint32 dwMs);

#ifdef _cplusplus
}
#endif

#endif 


