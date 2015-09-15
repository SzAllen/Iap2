
#include "Archdef.h"
#include "iAp2Session.h"
#include "Iap2CtrlSession.h"

extern Iap2Link* g_pIap2Link;

Bool SessionMgr_IsIdle(SessionMgr* pSessionMgr)
{
	return (IAP2_STATE_SESSION_INIT == pSessionMgr->m_State);
}

Bool SessionMgr_RxReq(SessionMgr* pSessionMgr)
{
	if(IAP2_STATE_SESSION_INIT != pSessionMgr->m_State)
	{
		return False;
	}
	
	Iap2Link_RcvData(g_pIap2Link, g_pIap2Link->m_pReqPkt, sizeof(Iap2LinkReqPkt));
	pSessionMgr->m_State = IAP2_STATE_RX_REQ;

	return True;
}

Bool SessionMgr_TxReq(SessionMgr* pSessionMgr, const void* pParam, uint16 len)
{
	if(IAP2_STATE_SESSION_INIT != pSessionMgr->m_State)
	{
		return False;
	}
	
	pSessionMgr->m_State = IAP2_STATE_TX_REQ;
	
	return True;
}

void SessionMgr_FsmReset(SessionMgr* pSessionMgr)
{
	//Stop the timer.
	Iap2Link_TimerReset(g_pIap2Link);
	pSessionMgr->m_State = IAP2_STATE_SESSION_INIT;
}

int SessionMgr_FsmTxReq(SessionMgr* pSessionMgr, Iap2Event nEventId, const Iap2LinkPkt* pPkt)
{
	PF_FUN_LINE(DL_MAIN);
	
	if(nEventId == IAP2_EVENT_TRANSFER_SUCCESS)
	{
		if(Iap2Link_TxData(g_pIap2Link, pSessionMgr, IAP2_RSP, 0, 0))
		{
			pSessionMgr->m_State = IAP2_STATE_RX_RSP;
			return IAP2RC_SUCCESS;
		}
	}

	return IAP2RC_FAILED;
}

int SessionMgr_FsmRxReq(SessionMgr* pSessionMgr, Iap2Event nEventId, const Iap2LinkPkt* pPkt)
{
	int nRc = IAP2RC_SUCCESS;
	uint16 nLen = SWAP16(pPkt->m_Len);
	CtrlByte ctrlByte;

	PF_FUN_LINE(DL_MAIN);
	
	if(nEventId != IAP2_EVENT_TRANSFER_SUCCESS) return IAP2RC_FAILED;

	memcpy(g_pIap2Link->m_pReqPkt, pPkt, nLen);

	if(nLen > IAP2_PKT_HEAD_SIZE)
	{
		nRc = pSessionMgr->ReqProc(pSessionMgr, g_pIap2Link->m_pReqPkt->m_Payload, g_pIap2Link->m_pRspPkt->m_Payload, &nLen);
	}
	else
	{
		PF_WARNING(("Receive data is invalid.\n"));
		Iap2Link_RcvData(g_pIap2Link, g_pIap2Link->m_pReqPkt, sizeof(Iap2LinkReqPkt));
		return IAP2RC_SUCCESS;
	}

	if(0 == nLen)
	{
		nLen = Iap2Link_AckInit(g_pIap2Link->m_pAckPkt, pPkt);
		Iap2Link_TxData(g_pIap2Link, g_pIap2Link->m_pAckPkt, nLen, 0, 0);
		pSessionMgr->m_State = IAP2_STATE_TX_ACK;
	}
	else
	{
		AS_UINT8(ctrlByte) = 0;
		ctrlByte.m_AckBit = 1;
		nLen = Iap2LinkPkt_Init(g_pIap2Link->m_pRspPkt
			, ctrlByte
			, g_pIap2Link->m_NextSentPSN++
			, pPkt->m_SeqNum
			, pPkt->m_SessionId
			, g_pIap2Link->m_pRspPkt->m_Payload
			, nLen
			);
		Iap2Link_TxData(g_pIap2Link, g_pIap2Link->m_pRspPkt, nLen, 0, 0);		
		pSessionMgr->m_State = IAP2_STATE_TX_RSP;
	}
	
	return IAP2RC_SUCCESS;
}

int SessionMgr_FsmTxRsp(SessionMgr* pSessionMgr, Iap2Event nEventId, const Iap2LinkPkt* pPkt)
{
	int nLen = SWAP16(pPkt->m_Len);
	
	PF_FUN_LINE(DL_MAIN);
	if(nEventId != IAP2_EVENT_TRANSFER_SUCCESS) return IAP2RC_FAILED;

	if(Iap2Link_RcvData(g_pIap2Link, g_pIap2Link->m_pAckPkt, sizeof(Iap2LinkReqPkt)))
	{
		pSessionMgr->m_State = IAP2_STATE_RX_ACK;
		return IAP2RC_SUCCESS;
	}
	return IAP2RC_FAILED;
}

int SessionMgr_FsmRxRsp(SessionMgr* pSessionMgr, Iap2Event nEventId, const Iap2LinkPkt* pPkt)
{
	int nLen = SWAP16(pPkt->m_Len);
	
	PF_FUN_LINE(DL_MAIN);
	if(nEventId != IAP2_EVENT_TRANSFER_SUCCESS) return IAP2RC_FAILED;

	if(pPkt->m_AckSeqNum != g_pIap2Link->m_pReqPkt->m_SeqNum)
	{
		PF_WARNING(("AckSeqNum[0x%x][0x%x] is not match.\n", pPkt->m_AckSeqNum, g_pIap2Link->m_pReqPkt->m_SeqNum));
		return IAP2RC_FAILED;
	}

	memcpy(g_pIap2Link->m_pRspPkt, pPkt, nLen);
	if(nLen == IAP2_PKT_HEAD_SIZE)
	{
		//Rcv ACK
		SessionMgr_FsmReset(pSessionMgr);
		pSessionMgr->EventProc(pSessionMgr, IAP2_EVENT_SESSION_DONE);
	}
	else	//Rcv RSP, Send ACK
	{
		nLen = Iap2Link_AckInit(g_pIap2Link->m_pAckPkt, pPkt);
		Iap2Link_TxData(g_pIap2Link, g_pIap2Link->m_pAckPkt, nLen, 0, 0);
		pSessionMgr->m_State = IAP2_STATE_TX_ACK;
	}
	
	return IAP2RC_SUCCESS;
}

Iap2Rc SessionMgr_FsmRxAck(SessionMgr* pSessionMgr, Iap2Event nEventId, const Iap2LinkPkt* pPkt)
{
	PF_FUN_LINE(DL_MAIN);
	if(nEventId != IAP2_EVENT_TRANSFER_SUCCESS) return IAP2RC_FAILED;
	
	if(pPkt->m_AckSeqNum != g_pIap2Link->m_pRspPkt->m_SeqNum
		|| pPkt->m_SeqNum != g_pIap2Link->m_pRspPkt->m_AckSeqNum)
	{
		PF_WARNING(("AckSeqNum[0x%x][0x%x] is not match.\n", pPkt->m_AckSeqNum, g_pIap2Link->m_pReqPkt->m_SeqNum));
		return IAP2RC_FAILED;
	}
	SessionMgr_FsmReset(pSessionMgr);
	pSessionMgr->EventProc(pSessionMgr, IAP2_EVENT_SESSION_DONE);
	return IAP2RC_SUCCESS;
}

Iap2Rc SessionMgr_FsmTxAck(SessionMgr* pSessionMgr, Iap2Event nEventId, const Iap2LinkPkt* pPkt)
{
	PF_FUN_LINE(DL_MAIN);
	if(nEventId != IAP2_EVENT_TRANSFER_SUCCESS) return IAP2RC_FAILED;
	
	if(nEventId == IAP2_EVENT_TRANSFER_SUCCESS)
	{
		SessionMgr_FsmReset(pSessionMgr);
		pSessionMgr->EventProc(pSessionMgr, IAP2_EVENT_LINK_SYNC_DONE);
	}
	return IAP2RC_SUCCESS;
}


Iap2Rc SessionMgr_Fsm(SessionMgr* pSessionMgr, Iap2Event nEventId, const Iap2LinkPkt* pPkt)
{
	#define FSM(state, func)	\
		if(state == pSessionMgr->m_State)	\
		{	\
			return func(pSessionMgr, nEventId, pPkt);	\
		}

	if(IAP2_EVENT_TRANSFER_SUCCESS != nEventId)
	{
		SessionMgr_FsmReset(pSessionMgr);
		pSessionMgr->EventProc(pSessionMgr, nEventId);
		return IAP2RC_SUCCESS;
	}

	FSM(IAP2_STATE_TX_REQ, SessionMgr_FsmTxReq);
	FSM(IAP2_STATE_RX_REQ, SessionMgr_FsmRxReq);
	FSM(IAP2_STATE_TX_RSP, SessionMgr_FsmTxRsp);
	FSM(IAP2_STATE_RX_RSP, SessionMgr_FsmRxRsp);
	FSM(IAP2_STATE_TX_ACK, SessionMgr_FsmTxAck);
	FSM(IAP2_STATE_RX_ACK, SessionMgr_FsmRxAck);

	return IAP2RC_UNKNOWN;
}

void SessionMgr_Init(SessionMgr* pSessionMgr)
{
	memset(pSessionMgr, 0, sizeof(SessionMgr));

	pSessionMgr->m_State = IAP2_STATE_SESSION_INIT;
}


