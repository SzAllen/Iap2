
#include "ArchDef.h"
#include "Iap2CtrlSession.h"

extern int IicDrv_Read(uint8 cmd, uint8* pBuf, uint8 len);
extern Iap2Link* g_pIap2Link;

void Iap2CtrlSessionMsg_swap(Iap2CtrlSessionMsg* pMsg)
{
	
	uint16 i = 0;
	uint8* pByte = (uint8*)&pMsg->m_Param;
	Iap2CtrlSessionMsgParam* pParam = Null;
	
	pMsg->m_Len   = SWAP16(pMsg->m_Len);
	pMsg->m_MsgId = SWAP16(pMsg->m_MsgId);

	while(i < pMsg->m_Len)
	{
		pParam = (Iap2CtrlSessionMsgParam*)&pByte[i];

		pParam->m_Len 	= SWAP16(pParam->m_Len);
		pParam->m_MsgId = SWAP16(pParam->m_MsgId);

		i += pParam->m_Len;
	}
}

int Iap2CtrlSessionMsg_Init(Iap2CtrlSessionMsg* pMsg, uint16 MsgId)
{
	memset(pMsg, 0, sizeof(Iap2CtrlSessionMsg));

	pMsg->m_Sof = IAP2_CTRL_SESSION_MSG_SOM;
	
	pMsg->m_Len   = SWAP16(IAP2_CTRL_SESSION_MSG_HEAD_SIZE);
	
	pMsg->m_MsgId   = SWAP16(MsgId);

	return IAP2_CTRL_SESSION_MSG_HEAD_SIZE;
}

int Iap2CtrlSessionMsg_AddParam(Iap2CtrlSessionMsg* pMsg
	, uint16 paramId
	, const uint8* pData
	, uint16 len)
{
	uint8* pByte = (uint8*)pMsg;
	uint16 i = SWAP16(pMsg->m_Len);
	
	pByte[i++] = U16_MSB(len);		//Parameter Length MSB
	pByte[i++] = U16_LSB(len);		//Parameter Length LSB
	
	pByte[i++] = U16_MSB(paramId);	//Parameter ID MSB
	pByte[i++] = U16_LSB(paramId);	//Parameter ID LSB
	
	memcpy(&pByte[i], pData, len);	//Parameter Data
	
	i += len;
	
	pMsg->m_Len = SWAP16(i);

	return i;
}

static int Iap2CtrlSession_AuthenticationCerticate(const Iap2CtrlSessionMsg* pReq, uint8* pData, uint16* pLen)
{
    uint16 page_num = 0;
	uint16 i, len = 0;
	uint8 j = 0;
        	
	*pLen = 0;
    //read data from MFI IC via IIC
    if(!IicDrv_Read(0x30, pData, 2))
    {
		PF_ERROR(("IIC Read data(2) error.\n"));
		goto End;
    }
    
    len = (pData[0] << 8) + pData[1];
    if(len > 1024)
    {
        len = 1024;
    }
	
    page_num = len / MFI_IC_PAGE_SIZE;
    if(len % 128)
    {
        page_num ++;
    }
    
    j = 0x31;
    for(i = 0; i < page_num; i++)
    {
        if(!IicDrv_Read(j, pData + (128 * i), 128))
    	{
			PF_ERROR(("IIC Read Page[%d] len[128] error.\n", i));
			goto End;
        }
        j++;
    }

	*pLen = len;

	return RC_SUCCESS;
End:
	return RC_FAILED;
}

static int Iap2CtrlSession_ChallengeRsp(const Iap2CtrlSessionMsg* pReq, uint8* pData, uint16* pLen)
{
	* pLen = 0;
	return IAP2RC_SUCCESS;
}

static int Iap2CtrlSession_GetIDInfo(const Iap2CtrlSessionMsg* pReq, uint8* pData, uint16* pLen)
{
	* pLen = 0;
	return IAP2RC_SUCCESS;
}

int Iap2CtrlSession_EventProc(Iap2CtrlSession* pSession, Iap2Event eventId)
{
	PF_VAR(DL_MAIN, eventId);
	
	if(IAP2_EVENT_SESSION_DONE == eventId)
	{
		if(IAP2_REQ_AUTH == pSession->m_MsgId
			|| IAP2_REQ_AUTH_CHA_RSP == pSession->m_MsgId
			|| IAP2_RSP_AUTH_RESULT_SUCCESS == pSession->m_MsgId)
		{
			SessionMgr_RxReq((SessionMgr*)pSession);
		}
		else if(IAP2_REQ_ID_ACCEPTED == pSession->m_MsgId)
		{
			SessionMgr_TxReq((SessionMgr*)pSession, Null, 0);
		}
	}
	else if(IAP2_EVENT_LINK_SYNC_DONE == eventId)
	{
		SessionMgr_RxReq((SessionMgr*)pSession);
	}
	else if(IAP2_EVENT_TRANSFER_FAILED == eventId)
	{
		//Break the session
	}
	return IAP2RC_SUCCESS;
}

int Iap2CtrlSession_ReqProc(Iap2CtrlSession* pSession, const Iap2CtrlSessionMsg* pReq, Iap2CtrlSessionMsg* pRsp, uint16* pLen)
{	
	uint16 nRspLen = 0;
	int nRet = RC_UNKNOWN;
	uint16 msgId = SWAP16(pReq->m_MsgId);

	*pLen = 0;
	Iap2CtrlSessionMsg_Init(pRsp, msgId + 1);

	pSession->m_MsgId = msgId;
	if(IAP2_REQ_AUTH == msgId)
	{
		Iap2CtrlSession_AuthenticationCerticate(pReq, (uint8*)pRsp->m_Param, &nRspLen);
	}
	else if(IAP2_REQ_AUTH_CHA_RSP == msgId)
	{
		Iap2CtrlSession_ChallengeRsp(pReq, (uint8*)pRsp->m_Param, &nRspLen);
	}
	else if(IAP2_RSP_AUTH_RESULT_SUCCESS == msgId)
	{
		//No Response
	}
	else if(IAP2_REQ_ID == msgId)
	{
		Iap2CtrlSession_GetIDInfo(pReq, (uint8*)pRsp->m_Param, &nRspLen);
	}
	else if(IAP2_REQ_ID_ACCEPTED == msgId)
	{
		//No Response
	}
	
	if(nRspLen)
	{
		*pLen = Iap2CtrlSessionMsg_AddParam(pRsp, 0, (uint8*)pRsp->m_Param, nRspLen);
	}

	return IAP2RC_SUCCESS;
}

int Iap2CtrlSession_RspProc(Iap2CtrlSession* pSession, const Iap2CtrlSessionMsg* pReq, const Iap2CtrlSessionMsg* pRsp)
{
	return IAP2RC_SUCCESS;
}

Bool Iap2CtrlSession_SendMsg(Iap2CtrlSession* pIap2CtrlSession, Iap2CtrlSessionMsg* pMsg)
{
	return Iap2Link_TxData(g_pIap2Link, pMsg, pMsg->m_Len, 0, 0);
}

Bool Iap2CtrlSession_Init(Iap2CtrlSession* pIap2CtrlSession)
{
	SessionIden* pSessionIden = (SessionIden*)pIap2CtrlSession;
	SessionMgr* pSession = (SessionMgr*)pIap2CtrlSession;

	memset(pIap2CtrlSession, 0, sizeof(Iap2CtrlSession));
	SessionMgr_Init(pSession);
	
	pSessionIden->m_ID   = CONTROL_SESSION_ID;
	pSessionIden->m_Type = CONTROL_SESSION_TYPE;
	pSessionIden->m_Ver  = CONTROL_SESSION_VERSION;
	
	pSession->EventProc = (Iap2EventFn)Iap2CtrlSession_EventProc;
	pSession->ReqProc 	= (Iap2ReqFn)Iap2CtrlSession_ReqProc;
	pSession->RspProc 	= (Iap2RspFn)Iap2CtrlSession_RspProc;

	Iap2Link_RegSession(g_pIap2Link, pSession);

	return True;
}

