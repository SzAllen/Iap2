
#include "Arch.h"
#include "Shell.h"

static Arch __g_Arch;
Arch* g_pArch = &__g_Arch;

Bool Arch_PostMsg(void* pObj, uint32 msgID, uint32 param1, uint32 param2)
{
	Bool bRet = False;
	Message msg;

	msg.m_pHandler 	= pObj;
	msg.m_MsgID 	= msgID;
	msg.m_Param1 	= param1;
	msg.m_Param2 	= param2;
	
	LOCK();
	bRet = QUEUE_add(&g_pArch->m_MsgQueue, &msg, sizeof(Message));
	UNLOCK();
	
	return bRet;
}

int Arch_FsmProc(void* pObj, uint32 state, uint8 eventId, void* pData, int len, const FsmMap* pFsmMapArray, int count)
{
	int i = 0;
	const FsmMap* pFsmMap = pFsmMapArray;

	for(i = 0; i < count; i++, pFsmMap++)
	{
		if(state == pFsmMap->m_State)
		{
			return pFsmMap->Proc(pObj, eventId, pData, len);
		}
	}

	return RC_DONOTHING;
}

void Arch_MsgIfAdd(MsgIf* pMsgIf)
{
	if(Null == g_pArch->m_pMsgIfList)
	{
		g_pArch->m_pMsgIfList = (List *)pMsgIf;
	}
	else
	{
		List_AddTail(g_pArch->m_pMsgIfList, (List *)pMsgIf);
	}
}

void Arch_MsgIfRemove(MsgIf* pMsgIf)
{
	if(List_isIn((List*)g_pArch->m_pMsgIfList, (List*)pMsgIf))
	{
		g_pArch->m_pMsgIfList = List_Remove((List*)pMsgIf);
	}
}
#if 0
Bool Arch_TimerIsStart(uint8 timerId)
{
	SwTimer* pTimer = TimerArray_Get(g_pArch->m_TimerPool, MAX_TIMER_COUNT, timerId);

	if(pTimer)
	{
		return pTimer->m_isStart;
	}
	else
	{
		return False;
	}
}

void Arch_TimerTimeOut(SwTimer* pTimer, MsgIf* pMsgIf)
{
	pTimer->m_isStart = False;
	if(pMsgIf->MsgHandler)
	{
		Arch_PostMsg(pMsgIf, MSG_TIMEOUT, pTimer->m_nTimerId, 0);
	}	
	else
	{
		SwTimer_Release(pTimer);
	}
}

Bool Arch_TimerReset(uint8 timerId)
{
	SwTimer* pTimer = TimerArray_Get(g_pArch->m_TimerPool, MAX_TIMER_COUNT, timerId);
	
	if(pTimer)
	{
		SwTimer_Reset(pTimer);
		return True;
	}
	else
	{
		PF_WARNING(("No nTimerId=%d\n", timerId));
		return False;
	}
}

void Arch_TimerRelease(uint8 timerId)
{
	SwTimer* pTimer = TimerArray_Get(g_pArch->m_TimerPool, MAX_TIMER_COUNT, timerId);

	if(pTimer)
	{
		SwTimer_Release(pTimer);
	}
}

void Arch_TimerStop(uint8 timerId)
{
	SwTimer* pTimer = TimerArray_Get(g_pArch->m_TimerPool, MAX_TIMER_COUNT, timerId);

	if(pTimer)
	{
		SwTimer_Stop(pTimer);
	}
}

void Arch_TimerStart(uint8 timerId, uint32 value_ms, MsgIf* pMsgIf)
{
	SwTimer* pTimer = TimerArray_Get(g_pArch->m_TimerPool, MAX_TIMER_COUNT, timerId);

	ASRT(timerId != INVALID_TIMER_ID);

	if(Null == pTimer)
	{
		pTimer = TimerArray_New(g_pArch->m_TimerPool, MAX_TIMER_COUNT);
	}
	
	ASRT(pTimer);
	
	if(Null == pTimer->m_pTimerManager)
	{
		SwTimer_Init(pTimer, (TimeoutFun)Arch_TimerTimeOut, pMsgIf);
		TimerManager_AddTimer(&g_pArch->m_TimerManager, pTimer);
	}
	else
	{
		pTimer->m_context = pMsgIf;
		pTimer->timeout = (TimeoutFun)Arch_TimerTimeOut;
	}
	
	SwTimer_Start(pTimer, timerId, value_ms);
}

void Arch_TimerTask(uint32 ticks)
{
#ifdef TIMER_TEST
	uint32 tick1;
	uint32 tick2;
#endif	
	static Bool isFirst = True;
	Arch* pArch = g_pArch;
	//uint32 ticks = ticks;
	if(ARCH_INIT == g_pArch->m_State) return;
	
	if(isFirst)
	{
		isFirst = False;
		TimerManager_ResetTimer(&pArch->m_TimerManager, ticks);
	}
	
	//while(1)
	{
		//ticks = GET_TICKS();
		TimerManager_Run(&pArch->m_TimerManager, ticks);

#ifdef TIMER_TEST
		tick1 = GET_TICKS();
#endif		
		SLEEP(TIMER_TASK_INTERVAL);

#ifdef TIMER_TEST
		tick2 = GET_TICKS();
		Printf("tick2=%d, tick1=%d, INTERVAL(%d) = tick2-tick1=%d\n", tick2, tick1, TIMER_TASK_INTERVAL, tick2-tick1);
#endif		
	}
}

void Arch_TimerTaskCreate()
{
	//uint32* pStackBuf = Null;
	//int nSize = 0;

	//Osa_CreateTask((TaskFun)Arch_TimerTask, g_pArch, "ArchTimer", pStackBuf, nSize);
}
#endif
void Arch_Run(Bool isEndless)
{
	Message* pMsg = Null;
	MsgIf* pMsgIf = Null;
	List* pNode = Null;
	
	//PF_FUN_LINE(DL_MAIN);
	g_pArch->m_State = ARCH_RUN;
	
    do
    {			
		pMsg = (Message*)QUEUE_getHead(&g_pArch->m_MsgQueue);
		if(pMsg) 
		{
			pMsgIf = (MsgIf*)pMsg->m_pHandler;
			ASRT(pMsgIf && pMsgIf->MsgHandler);
			if(pMsgIf && pMsgIf->MsgHandler)
			{
				pMsgIf->MsgHandler(pMsgIf, pMsg->m_MsgID, pMsg->m_Param1, pMsg->m_Param2);
			}
			
			LOCK();
            QUEUE_removeHead(&g_pArch->m_MsgQueue);
			UNLOCK();
		}

		for(pNode = (List*)g_pArch->m_pMsgIfList; Null != pNode; pNode = pNode->m_pNext)
		{
			pMsgIf = (MsgIf*)pNode;
			if(pMsgIf->Run)
			{
				pMsgIf->Run(pMsgIf);
			}
		}
		Shell_Run();
		//SLEEP(1);
    }while(isEndless);
}

int Arch_Start()
{
	//Arch_TimerTaskCreate();
	g_pArch->m_State = ARCH_START;

	return RC_SUCCESS;
}

int Arch_Init()
{
	//MsgIf* pMsgIf = (MsgIf*)g_pArch;
	
	memset(g_pArch, 0, sizeof(Arch));
		
	Debug_Init();
	Shell_Init();
	
	PF_FUN_LINE(DL_MAIN);

	TimerManager_Init(&g_pArch->m_TimerManager);
	
	//Init Message Queue
	QUEUE_init(&g_pArch->m_MsgQueue
		, (uint8*)&g_pArch->m_MessageBuff
		, sizeof(Message)
		, MAX_MSG_COUNT
		);

	Osa_Init();
	
	g_pArch->m_State = ARCH_INIT;
	return RC_SUCCESS;
}

