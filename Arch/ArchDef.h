#ifndef __ARCH_CFG_H_
#define __ARCH_CFG_H_

#ifdef __cplusplus
extern "C"{
#endif

#define  SECTOR_SIZE        512

#define MAX_MSG_COUNT		60	//Message queue deepth
#define MAX_TIMER_COUNT		4
#define MS_STICKS 			1	//Hot many ticks per ms, it is for timer
#define TIMER_TASK_INTERVAL 1
#define MAX_TEMP_BUF_SIZE 	SECTOR_SIZE

#ifdef _WINDOWS
	#include <windows.h>
	#include <stdio.h>

	#define SLEEP 		Sleep
	#define LOCK(...)	//{if(g_CriticalSection.OwningThread) EnterCriticalSection(&g_CriticalSection)
	#define UNLOCK(...)	//if(g_CriticalSection.OwningThread) LeaveCriticalSection(&g_CriticalSection);}
	#define GET_TICKS() GetTickCount()
	
	extern CRITICAL_SECTION g_CriticalSection;
#else
	extern void Mcu_Sleep(unsigned int ms);
	#define SLEEP 		Mcu_Sleep
	
	#define LOCK()		{irqflags_t flags; flags = cpu_irq_save()	//{Enable_global_interrupt();
	#define UNLOCK()	cpu_irq_restore(flags);}	//Disable_global_interrupt();}

	extern unsigned int system_ms_tick;
	#define GET_TICKS( ) system_ms_tick
#endif

#include "typedef.h"	
#include "Debug.h"	
#include "String.h"
#include "MsgDef.h"
#include "RetCode.h"
#include <stdio.h>
#include "Queue.h"
#include "Message.h"
#include "Str.h"


typedef int (*EventProcFun)(void* pObj, uint8 eventId, void* pData, int len);
typedef struct _FsmMap
{
	uint8 			m_State;
	EventProcFun	Proc;
}FsmMap;


typedef struct _ArchDef
{
	uint8 id;
}ArchDef;

typedef void (*TaskFun)(void* pArg);
typedef void (*voidObjFun)(void* pObj);
typedef int (*ObjFun)(void* pObj);
typedef int (*ObjIsFun)(void* pObj);
typedef int (*ObjSetFun)(void* pObj, uint32 value);
typedef Bool (*EnableFun)(void* pObj, Bool isEnable);

extern Bool Arch_PostMsg(void* pObj, uint32 msgID, uint32 param1, uint32 param2);
extern Bool Arch_PostMsg(void* pObj, uint32 msgID, uint32 param1, uint32 param2);
extern uint8 g_TempBuff[MAX_TEMP_BUF_SIZE];
int Osa_Init();

#define READ_REG32(reg) (uint32)(*(vuint32*)(reg))
#define WRITE_REG32(reg, vvalue) (*(vuint32*)(reg)) = vvalue
#define MODIFY_REG32(reg, clear_mask, set_mask) ((*(vuint32*)(reg)) = ((*(vuint32*)(reg)) &~ clear_mask) | set_mask)

#define MAX_SERIAL_NUM_SIZE 32
#define SC_SERIAL_NUM_SIZE	12

#ifdef __cplusplus
}
#endif

#endif




