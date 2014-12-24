/******************************************************************************
Main.h 
`
描述:
1. 实现项目入口函数功能

创建:
林良军   2013.09.3    建立文件.
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


