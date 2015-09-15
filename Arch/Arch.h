#ifndef  _DEBUG_H_
#define  _DEBUG_H_

#ifdef __cplusplus
extern "C"{
#endif	//#ifdef __cplusplus

#include "ArchDef.h"
#include "Queue.h"
#include "SwTimer.h"
#include "Message.h"

enum
{
	 ARCH_INIT = 0
	,ARCH_START
	,ARCH_RUN
};

typedef struct _Arch
{
	MsgIf m_Base;

	uint8	m_State;
	
	List* m_pMsgIfList;
	
	SwTimer			m_TimerPool[MAX_TIMER_COUNT];
	TimerManager	m_TimerManager;
	
	Queue	m_MsgQueue;
	Message	m_MessageBuff[MAX_MSG_COUNT];
}Arch;

int Arch_Init(void);
int Arch_Start(void);
void Arch_Run(Bool isEndless);
Bool Arch_PostMsg(void* pObj, uint32 msgID, uint32 param1, uint32 param2);

void Arch_MsgIfAdd(MsgIf* pMsgIf);
void Arch_MsgIfRemove(MsgIf* pMsgIf);

void Arch_TimerRelease(uint8 timerId);
Bool Arch_TimerIsStart(uint8 timerId);
void Arch_TimerStop(uint8 timerId);
void Arch_TimerStart(uint8 timerId, uint32 value_ms, MsgIf* pMsgIf);
Bool Arch_TimerReset(uint8 timerId);

int Arch_FsmProc(void* pObj
	, uint32 state
	, uint8 eventId
	, void* pData
	, int len
	, const FsmMap* pFsmMapArray, int count);

#ifdef __cplusplus
}
#endif	//#ifdef __cplusplus

#endif //#ifndef  _DEBUG_H_

