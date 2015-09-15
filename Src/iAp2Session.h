#ifndef __IAP2_SESSION_H_
#define __IAP2_SESSION_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "Typedef.h"
#include "Iap2Link.h"

//iAP2 SessionTypes
#define SESSION_TYPE_CTRL	0	//Controlsession
#define SESSION_TYPE_FILE	1	//File Transfersession
#define SESSION_TYPE_ACCE	2	//External Accessory session

typedef enum _SessionState
{
	 IAP2_STATE_SESSION_INIT = IAP2_STATE_RX_ACK + 1
	,IAP2_STATE_TX_REQ
	,IAP2_STATE_RX_REQ
	,IAP2_STATE_TX_RSP
	,IAP2_STATE_RX_RSP
}SessionState;



typedef int (*Iap2EventFn)(void* pObj, Iap2Event event);
typedef int (*Iap2ReqFn)(void* pObj, const void* pReq, void* pRsp, uint16* pLen);
typedef int (*Iap2RspFn)(void* pObj, const void* pReq, void* pRsp);

typedef struct _SessionMgr
{
	SessionIden 	m_Base;
	
	uint8			m_State;

	Iap2EventFn		EventProc;
	Iap2ReqFn 		ReqProc;
	Iap2RspFn 		RspProc;
}SessionMgr;
	
Iap2Rc SessionMgr_Fsm(SessionMgr* pSessionMgr, Iap2Event nEventId, const Iap2LinkPkt* pPkt);
Bool SessionMgr_RxReq(SessionMgr* pSessionMgr);
Bool SessionMgr_TxReq(SessionMgr* pSessionMgr, const void* pParam, uint16 len);
void SessionMgr_Init(SessionMgr* pSessionMgr);

#ifdef __cplusplus
}
#endif

#endif


