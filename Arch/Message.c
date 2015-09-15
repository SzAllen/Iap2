#include "Message.h"

void MsgIf_Init(MsgIf* pMsgIf, MsgProcFun MsgHandler, RunFun Run)
{
	pMsgIf->MsgHandler = MsgHandler;
	pMsgIf->Run = Run;
}

int MsgIf_MsgProc(void* pObj, uint8 msgId, uint32 param1, uint32 param2, const MsgMap* pMsgTbl, int nCount)
{
	int i = 0;
	
	for(i = 0; i < nCount; i++, pMsgTbl++)
	{
		if(msgId == pMsgTbl->m_MsgID)
		{
			return pMsgTbl->MsgHandler((MsgIf*)pObj, param1, param2);
		}
	}

	return -1;
}

