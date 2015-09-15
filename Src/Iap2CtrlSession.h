#ifndef __IAP2_CTRL_SESSION__H_
#define __IAP2_CTRL_SESSION__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "Iap2Session.h"

#define CONTROL_SESSION_ID     	0x0a
#define CONTROL_SESSION_TYPE   	0x00
#define CONTROL_SESSION_VERSION	0x01
#define CONTROL_SESSION_SOF 	0x4040

#define IAP2_CTRL_SESSION_MSG_PARAM_HEAD_SIZE 	4

#define IAP2_CTRL_SESSION_MSG_HEAD_SIZE 		6
#define IAP2_CTRL_SESSION_MSG_SOM				0x4040
#define IAP2_SESSION_MSG_SIZE					512

#define MFI_IC_PAGE_SIZE                 128

//Accessory Authentication
#define IAP2_REQ_AUTH 					0xAA00
#define IAP2_RSP_AUTH 					0xAA01
#define IAP2_REQ_AUTH_CHA_RSP 			0xAA02
#define IAP2_RSP_AUTH_CHA_RSP 			0xAA03
#define IAP2_RSP_AUTH_RESULT_FAILED 	0xAA04
#define IAP2_RSP_AUTH_RESULT_SUCCESS 	0xAA05

//Accessory Identification
#define IAP2_REQ_ID 			0x1D00
#define IAP2_REQ_ID_ACCEPTED	0x1D02
#define IAP2_REQ_ID_REJECTED	0x1D03
#define IAP2_REQ_ID_CANCEL		0x1D05
#define IAP2_REQ_ID_UPDATED		0x1D06

//Power
#define IAP2_REQ_POWER_START_UPDATE		0xAE00
#define IAP2_REQ_POWER_UPDATE			0xAE01
#define IAP2_REQ_POWER_STOP_UPDATE		0xAE02
#define IAP2_REQ_POWER_SOURCE_UPDATE	0xAE02


/*
typedef int (Iap2ReqFn)(const Iap2CtrlSessionMsg* pReq, uint8* pData, uint16* pLen);
typedef struct _Iap2Req
{
}Iap2Req
*/

#pragma pack(1)

typedef struct _Iap2CtrlSessionMsgParam
{
	uint16 m_Len;
	uint16 m_MsgId;

	uint8 m_Data[1];
}Iap2CtrlSessionMsgParam;

typedef struct _Iap2CtrlSessionMsg
{
	uint16 m_Sof;
	uint16 m_Len;
	uint16 m_MsgId;
	Iap2CtrlSessionMsgParam m_Param[1];
}Iap2CtrlSessionMsg;

#pragma pack()

typedef struct _Iap2CtrlSession
{
	SessionMgr	m_Base;
	
	uint16 m_MsgId;

	Iap2CtrlSessionMsg m_TxMsg;
	Iap2CtrlSessionMsg m_RxMsg;
}Iap2CtrlSession;

Bool Iap2CtrlSession_Init(Iap2CtrlSession* pIap2CtrlSession);
Bool Iap2CtrlSession_SendMsg(Iap2CtrlSession* pIap2CtrlSession, Iap2CtrlSessionMsg* pMsg);

#ifdef __cplusplus
}
#endif

#endif

