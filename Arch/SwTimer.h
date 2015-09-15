#ifndef  _SWTIMER_H_
#define  _SWTIMER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "List.h"

#define INVALID_TIMER_ID	0xFF

struct _tagSwTimer;
typedef void (*TimeoutFun)(struct _tagSwTimer*, void* context);
struct _tagTimerManager;
typedef struct _tagSwTimer
{
	List	m_base;
    uint32 	m_dwTimeoutTicks;
    uint32 	m_dwInitTicks;
	
    uint8 	m_isStart:1;
    uint8 	m_Reserved:7;
    
    uint8 	m_nTimerId;
	
    void* 	m_context;
    TimeoutFun timeout;
	struct _tagTimerManager* m_pTimerManager;
}SwTimer;

typedef struct _tagTimerManager
{
    SwTimer* 	m_timerList;
	uint32		m_ticks;
}TimerManager;

void SwTimer_Init(SwTimer* pTimer, TimeoutFun timeout, void* pContext);
void SwTimer_Stop(SwTimer* pTimer);
void SwTimer_Start(SwTimer* pTimer, uint8 timerId, uint32 value_ms);
void SwTimer_ReStart(SwTimer* pTimer);
Bool SwTimer_isStart(SwTimer* pTimer);
void SwTimer_Reset(SwTimer* pTimer);
uint8 SwTimer_GetId(SwTimer* pTimer);
void SwTimer_Release(SwTimer* pTimer);
Bool SwTimer_isTimerOut(uint32 initTicks, uint32 newTicks, uint32 timeOutTicks);
Bool SwTimer_isTimerOutEx(SwTimer* pTimer);

SwTimer* TimerArray_New(SwTimer* pTimerArray, int nCount);
SwTimer* TimerArray_Get(SwTimer* pTimerArray, int nCount, uint8 timerId);

void TimerManager_Init(TimerManager* pTm);
void TimerManager_AddTimer(TimerManager* pTm, SwTimer* pTimer);
void TimerManager_Run(TimerManager* pTm, uint32 ticks);
void TimerManager_ResetTimer(TimerManager* pTm, uint32 ticks);
void TimerManager_RemoveTimer(SwTimer* pTimer);

#ifdef __cplusplus
}
#endif

#endif

