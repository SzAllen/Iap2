#ifndef __ARCH_CFG_H_
#define __ARCH_CFG_H_

#ifdef __cplusplus
extern "C"{
#endif

#define  SECTOR_SIZE        512

#define MAX_MSG_COUNT		16	//Message queue deepth
#define MAX_TIMER_COUNT		4
#define MS_STICKS 			1	//Hot many ticks per ms, it is for timer
#define TIMER_TASK_INTERVAL 1
#define MAX_TEMP_BUF_SIZE 	SECTOR_SIZE

#define CY_FX_MSC_CLR_STALL_IN_EVENT_FLAG       (1 << 0)        /* MSC application Clear Stall IN Event Flag */
#define CY_FX_MSC_RESET_EVENT_FLAG              (1 << 1)        /* USB reset event. */
#define CY_FX_MSC_READY_EVENT_FLAG              (1 << 2)        /* MSC ready event */
#define CY_FX_MSC_CBW_EVENT_FLAG                (1 << 3)        /* MSC CBW received. */
#define CY_FX_MSC_DATASENT_EVENT_FLAG           (1 << 4)        /* Data has been sent to the host. */
#define CY_FX_MSC_SIBCB_EVENT_FLAG              (1 << 5)        /* SIB event callback flag. */
#define CY_FX_MSC_USB_SUSP_EVENT_FLAG           (1 << 6)        /* USB Suspend event flag. */
#define CY_FX_PWR_CHANGE_EVENT_FLAG				(1 << 7)
#define CY_FX_IAP_READY_EVENT_FLAG				(1 << 8)
#define CY_FX_IAP_ERROR_EVENT_FLAG				(1 << 9)
#define CY_FX_IAP_DATA_RECV_EVENT_FLAG			(1 << 10)
#define CY_FX_IAP_DATA_SEND_EVENT_FLAG			(1 << 11)
#define CY_FX_MSC_DATAOUT_EVENT_FLAG			(1 << 12)
#define CY_FX_MSC_TRANS_FLAG           			(1 << 13)        /* Data has been sent to the host. */


#ifdef _WINDOWS
	#include <windows.h>
	#include <stdio.h>

	#define SLEEP 		Sleep
	#define LOCK(...)	//{if(g_CriticalSection.OwningThread) EnterCriticalSection(&g_CriticalSection)
	#define UNLOCK(...)	//if(g_CriticalSection.OwningThread) LeaveCriticalSection(&g_CriticalSection);}
	#define GET_TICKS() (GetTickCount()/2)
	
	extern CRITICAL_SECTION g_CriticalSection;
#else
	extern void delay_us(unsigned short nCnt);
	extern void delay_ms(unsigned short nCnt);
	extern void delay_s(unsigned short nCnt);
	#define SLEEP 		delay_ms
	
	#define LOCK(...)
	#define UNLOCK(...)

	extern unsigned int system_ms_tick;
	#define GET_TICKS( ) CyU3PGetTime()

	#include <stdio.h>
	#include <cyu3system.h>
	#include <cyu3os.h>
	#include <cyu3dma.h>
	#include <cyu3socket.h>
	#include <cyu3error.h>
	#include <cyu3usb.h>
	#include <cyu3usbconst.h>
	#include <cyu3uart.h>
	#include <cyu3sib.h>
	#include <cyu3types.h>
	#include <cyu3usbconst.h>
	#include <cyu3externcstart.h>
	#include <cyu3gpio.h>
	#include "UdcCfg.H"
	#include "cyu3system.h"
	#include <uart_regs.h>

	extern void CyFxAppErrorHandler (CyU3PReturnStatus_t apiRetStatus);
	extern CyU3PEvent  glAppEvent;

#endif

#include "typedef.h"	
#include "Debug.h"	
#include "String.h"
#include "RetCode.h"
#include "SwTimer.h"

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

#define MAX_MODULE_NUM_SIZE 32
#define SC_SERIAL_NUM_SIZE	12

#define MIN_VALUE(x,y) ((x) > (y)) ? (y) : (x)
#define MAX_VALUE(x,y) ((x) > (y)) ? (x) : (y)

#ifdef __cplusplus
}
#endif

#endif




