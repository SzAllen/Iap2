
#include "ArchDef.h"
#include "SwTimer.h"

void SwTimer_Init(SwTimer* pTimer, TimeoutFun timeout, void* pContext)
{
	memset(pTimer, 0, sizeof(SwTimer));

	pTimer->m_base.m_isUsed = 1;
	pTimer->m_context = pContext;
	pTimer->timeout = timeout;
	pTimer->m_nTimerId = 0;
}

void SwTimer_Start(SwTimer* pTimer, uint8 timerId, uint32 value_ms)
{
	ASRT(value_ms);
	
	pTimer->m_dwTimeoutTicks = value_ms * MS_STICKS;
	pTimer->m_dwInitTicks = GET_TICKS();
	pTimer->m_nTimerId = timerId;
	pTimer->m_isStart = 1;

	PF(DL_TIMER,("Init Timer(ID=%d), Timerout ticks=%d, initTicks=0x%08x\n", pTimer->m_nTimerId, pTimer->m_dwTimeoutTicks, pTimer->m_dwInitTicks));
}

void SwTimer_ReStart(SwTimer* pTimer)
{
	pTimer->m_dwInitTicks = GET_TICKS();
	pTimer->m_isStart = 1;
}

void SwTimer_Stop(SwTimer* pTimer)
{
	pTimer->m_isStart = 0;
}

void SwTimer_Release(SwTimer* pTimer)
{
	TimerManager_RemoveTimer(pTimer);
	memset(pTimer, 0, sizeof(SwTimer));
}


Bool SwTimer_isStart(SwTimer* pTimer)
{
	return pTimer->m_isStart;
}

Bool SwTimer_isTimerOutEx(SwTimer* pTimer)
{
	return SwTimer_isTimerOut(pTimer->m_dwInitTicks, GET_TICKS(), pTimer->m_dwTimeoutTicks);
}

Bool SwTimer_isTimerOut(uint32 initTicks, uint32 newTicks, uint32 timeOutTicks)
{
	uint32 totalTicks = 0;
	if(newTicks < initTicks)
	{
		//tick counter overflow
		totalTicks = 0xFFFFFFFF - initTicks + newTicks;
	}
	else
	{
		totalTicks = newTicks - initTicks;
	}
	
	return (totalTicks >= timeOutTicks);
}

void SwTimer_TimerOut(SwTimer* pTimer)
{
	ASRT(pTimer->timeout);
	pTimer->timeout(pTimer, pTimer->m_context);
}

void SwTimer_Reset(SwTimer* pTimer)
{
	if(pTimer->m_pTimerManager)
	{
		pTimer->m_dwInitTicks = pTimer->m_pTimerManager->m_ticks;
	}
}

uint8 SwTimer_GetId(SwTimer* pTimer)
{
	return pTimer->m_nTimerId;
}

////////////////////////////////////////////////////////////////////////////////
SwTimer* TimerArray_New(SwTimer* pTimerArray, int nCount)
{
	int i = 0; 

	for(i = 0; i < nCount; i++, pTimerArray++)
	{
		if(!pTimerArray->m_base.m_isUsed)
		{
			return pTimerArray;
		}
	}

	return Null;
}

SwTimer* TimerArray_Get(SwTimer* pTimerArray, int nCount, uint8 timerId)
{
	int i = 0; 

	for(i = 0; i < nCount; i++, pTimerArray++)
	{
		if(pTimerArray->m_nTimerId == timerId)
		{
			return pTimerArray;
		}
	}

	return Null;
}

////////////////////////////////////////////////////////////////////////////////

void TimerManager_Init(TimerManager* pTm)
{
	memset(pTm, 0, sizeof(TimerManager));
	#ifdef TIMER_TEST
	SwTimer_Test();
	#endif
}

void TimerManager_AddTimer(TimerManager* pTm, SwTimer* pTimer)
{
	pTimer->m_pTimerManager = pTm;
	if(Null == pTm->m_timerList)
	{
		pTm->m_timerList = pTimer;
	}
	else
	{
		List_AddTail((List*)pTm->m_timerList, (List*)pTimer);
	}
}	

void TimerManager_RemoveTimer(SwTimer* pTimer)
{
	TimerManager* pTm = pTimer->m_pTimerManager;
	
	if(Null == pTm || Null == pTm->m_timerList) return;
	
	if(List_isIn((List*)pTm->m_timerList, (List*)pTimer))
	{
		pTm->m_timerList = (SwTimer*)List_Remove((List*)pTimer);
	}
}

void TimerManager_ResetTimer(TimerManager* pTm, uint32 ticks)
{
	SwTimer* pTimer = (SwTimer*)pTm->m_timerList;

	pTm->m_ticks = ticks;
	for(;pTimer != Null; pTimer = (SwTimer*)(((List*)pTimer)->m_pNext))
	{
		if(SwTimer_isStart(pTimer))
		{
			SwTimer_Reset(pTimer);
		}
	}
}

void TimerManager_Run(TimerManager* pTm, uint32 ticks)
{
	//TimerManager* pTm = GetTimerManager();
	List* pListNode = (List*)pTm->m_timerList;
	SwTimer* pTimer = (SwTimer*)pListNode;
	Bool isTimeOut = False;

	pTm->m_ticks = ticks;
	
	if(Null == pListNode)
	{
		return;
	}
	
	do
	{
		isTimeOut = False;
		if(SwTimer_isStart(pTimer))
		{
			if(SwTimer_isTimerOutEx(pTimer))
			{
				//SwTimer_Stop(pTimer);
				
				isTimeOut = True;
				//Save the pListNode->pNext before calling SwTimer_TimerOut(), pListNode may be destoryed at SwTimer_TimerOut()
				pListNode = pListNode->m_pNext;	
				SwTimer_TimerOut(pTimer);
			}
		}

		if(!isTimeOut)
		{
			pListNode = pListNode->m_pNext;
		}
		pTimer = (SwTimer*)pListNode;
		
	}while(pTimer);
	
}
