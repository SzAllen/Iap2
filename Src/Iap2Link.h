#ifndef __IAP2_LINK__H_
#define __IAP2_LINK__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "SwTimer.h"
//#include "iAp2Session.h"

//////////////////////////////////////////////////////
#define IAP2_PKT_SOF_MSB			0XFF
#define IAP2_PKT_SOF_LSB			0X5A

#define IAP2_PKT_HEAD_SIZE			9

#define IAP2_LINK_VERSION 			0X01
#define IAP2_MAX_OUTSTANDING_PKT 	0X01
#define MAX_IAP2_REQ_PAYLOAD_SIZE	64
#define MAX_IAP2_RSP_PAYLOAD_SIZE	1024 + 50
#define IAP2_MAX_PACKET_SIZE 		(uint16)(32768+30)//(MAX_IAP2_RSP_PAYLOAD_SIZE + IAP2_PKT_HEAD_SIZE)
#define IAP2_MAX_ACK_TIMEOUT		(uint16)(0)
#define IAP2_MAX_RETX_COUNT			(10)
#define IAP2_RETX_TIMEOUT 			(uint16)1500
#define IAP2_RETX_INTERVAL 			1000

#define IAP2_MAX_SESSION_COUNT			2

#define MAX_RCV_OFS_PSN					3
#define IAP2_LINK_INIT_TIMERID			1
#define IAP2_LINK_RETX_COUNT_UNLIMITIED	0xFF
#define IAP2_MAX_PAYLOAD_COUNT			IAP2_MAX_SESSION_COUNT

#define LINK_SYNC_PKT_SIZE				23

#define IAP2_REQ 0
#define IAP2_RSP 1
#define IAP2_ACK 2

typedef struct _CtrlByte
{
	uint8 m_Reserved:3;	//Bit0-2: reserved, set to 0
	uint8 m_SlpBit:1;	//Bit3: Device Sleep
	uint8 m_RstBit:1;	//Bit4: Link Reset
	uint8 m_EakBit:1;	//Bit5: Extended Acknowledgement Payload is present
	uint8 m_AckBit:1;	//Bit6: Packet Acknowledgement Number is valid,and iAP2 SessionMgr Payload may be present
	uint8 m_SynBit:1;	//Bit7: Link Synchronization Payload is present
}CtrlByte;

typedef enum _PayloadType
{
	 PL_TYPE_DETECT_IAP2 = 0
	,PL_TYPE_LINK_SYNC
	,PL_TYPE_SESSION
	,PL_TYPE_EAK
	,PL_TYPE_UNKNOWN
}PayloadType;


#pragma pack(1)
typedef struct _Iap2LinkPkt
{
	uint16 m_Sof;
	uint16 m_Len;
	CtrlByte m_CtrlByte;
	
	uint8 m_SeqNum;
	uint8 m_AckSeqNum;	
	uint8 m_SessionId;
	uint8 m_HeaderSum;
	uint8 m_Payload[1];
}Iap2LinkPkt;

typedef struct _Iap2LinkReqPkt
{
	Iap2LinkPkt	m_Base;
	uint8 m_Payload[MAX_IAP2_REQ_PAYLOAD_SIZE];
}Iap2LinkReqPkt;

typedef struct _Iap2LinkRspPkt
{
	Iap2LinkPkt	m_Base;
	uint8 m_Payload[MAX_IAP2_RSP_PAYLOAD_SIZE];
}Iap2LinkRspPkt;

typedef struct _Iap2Req
{
	Iap2LinkPkt	m_Base;
	uint8 m_Payload[MAX_IAP2_REQ_PAYLOAD_SIZE];
}Iap2Req;

typedef struct _Iap2Rsp
{
	Iap2LinkPkt	m_Base;
	uint8 m_Payload[MAX_IAP2_RSP_PAYLOAD_SIZE];
}Iap2Rsp;

#pragma pack()

typedef struct _SessionIden
{
	uint8 m_ID;
	uint8 m_Type;
	uint8 m_Ver;
}SessionIden;


typedef struct _LinkPayload
{
	uint8 m_LinkVer;			//LinkVersion
	uint8 m_MaxOfOutstandingPkt;//Maximum Number of Outstanding Packets
	
	uint8 m_MaxPktLenMsb;	//Maximum Packet Length MSB
	uint8 m_MaxPktLenLsb;	//Maximum Packet Length LSB
	
	uint8 m_ReTxTimeOutMsb;	//Retransmission Timeout MSB
	uint8 m_ReTxTimeOutLsb;	//Retransmission Timeout LSB
	
	uint8 m_AckTimeOutMsb;	//Cumulative Acknowledgement Timeout MSB
	uint8 m_AckTimeOutLsb;	//Cumulative Acknowledgement Timeout LSB
	
	uint8 m_MaxReTxCount;	//Maximum Number of Retransmissions
	uint8 m_MaxAckCount;	//Maximum Cumulative Acknowledgements
	
	SessionIden m_Session[IAP2_MAX_SESSION_COUNT];
}LinkPayload;

#define LINK_PAYLOAD_HEAD_SIZE	(10)
void LinkPayload_Init(LinkPayload* pLinkPayload);

//////////////////////////////////////////////////////
typedef enum _Iap2Event
{
	IAP2_EVENT_INIT = 1
	,IAP2_EVENT_HAND_SHAKE
	,IAP2_EVENT_EAK 
	,IAP2_EVENT_ACK 
	,IAP2_EVENT_LINK_SYNC_DONE 
	,IAP2_EVENT_TRANSFER_SUCCESS
	,IAP2_EVENT_TRANSFER_FAILED
	,IAP2_EVENT_SESSION_DONE 
	,IAP2_EVENT_RX_REQ 
}Iap2Event;

typedef enum _Iap2Rc
{
	 IAP2RC_SUCCESS	=	0
	,IAP2RC_FAILED	=	1
	,IAP2RC_PENDING
	,IAP2RC_ACK
	,IAP2RC_UNKNOWN
}Iap2Rc;

typedef enum _Iap2State
{
	 IAP2_STATE_INIT = 0
	,IAP2_STATE_TX_LINK_SYNC
	,IAP2_STATE_RX_LINK_SYNC
	,IAP2_STATE_TX_ACK
	,IAP2_STATE_RX_ACK
	,IAP2_STATE_LINK_DONE
}Iap2State;

//typedef Bool (*IrbNotifyFn)(void* pParam, Irb* pIrb);
typedef struct _Irb
{
	uint8 	m_isTx;
	uint8*	m_pBuff;
	uint16	m_Len;
	uint16	m_ActLen;

//	IrbNotifyFn Notify;
//	void*		m_pParam;
}Irb;

typedef Bool (*Iap2LinkTransferFn)(Irb* pIrb);
struct _SessionMgr;
typedef struct _Iap2Link
{
	uint8	m_State;
//	uint8	m_PayLoadType;
	
	SwTimer	m_SentACKTimer;			//SentACKTimer: A timer that keeps track of the elapsed time (ms) since the last ACK packet was sent
	uint8 	m_NextSentPSN;			//NextSentPSN: The Packet Sequence Number of the next packet to be sent
	uint8 	m_OldestTxNakPSN;		//OldestSentUnacknowledgedPSN: The Packet Sequence Number of the oldest unacknowledged packet
	uint8 	m_InitialSentPSN;		//InitialSentPSN: The Packet Sequence Number used for the very first packet sent
	uint8 	m_LastRcvInPSN;			//LastReceivedInSequencePSN: The Packet Sequence Number of the last packet received correctly and in sequence
	uint8 	m_InitialRcvSN;			//InitialReceivedPSN: The Packet Sequence Number of the very first packet received
	uint8 	m_RcvPSNs[MAX_RCV_OFS_PSN];	//ReceivedOutOfSequencePSNs[n]:An array of Packet Sequence Numbers that have been received and acknowledged out of sequence

	struct _SessionMgr* m_pSessionMgr;

	uint8 	m_SessionCount;
	SessionIden m_Session[IAP2_MAX_SESSION_COUNT];
	
	Iap2LinkReqPkt  _ReqPkt;
	Iap2LinkRspPkt  _RspPkt;
	Iap2LinkPkt		_AckPkt;
	Iap2LinkPkt* 	m_pReqPkt;
	Iap2LinkPkt* 	m_pRspPkt;
	Iap2LinkPkt* 	m_pAckPkt;
	
	SwTimer	m_ReTxTimer;		//Re send timer.
	uint8	m_ReTxCount;		//Re send Count.
	uint8	m_MaxReTxCount;		//Max Re send Count.
	uint16	m_ReSendTimeMs;

	Irb		m_Irb;
	
	Iap2LinkTransferFn TransferStart;
}Iap2Link;

void Iap2Link_Init(Iap2Link* pIap2Link, Iap2LinkTransferFn Tx);
void Iap2Link_Start(Iap2Link* pIap2Link);
void Iap2Link_Run(Iap2Link* pIap2Link);
int Iap2LinkPkt_Init(Iap2LinkPkt* pPkt
	, CtrlByte ctrl
	, uint8 seq
	, uint8 ackSeq
	, uint8 sessionId
	, const void* pPaoload
	, uint16 payloadLen);

Bool Iap2Link_RcvData(Iap2Link* pIap2Link, void* pBuff, int len);
Bool Iap2Link_TxData(Iap2Link* pIap2Link, const void* pData, int len, uint32 reSendTimeMs, uint8 maxTxCount);
int Iap2Link_AckInit(Iap2LinkPkt* pAckPkt, const Iap2LinkPkt* pPkt);
void Iap2Link_RegSession(Iap2Link* pIap2Link, struct _SessionMgr* pSession);
void Iap2Link_TransferDone(Iap2Link* pIap2Link, Bool isSuccess);
void Iap2Link_TimerReset(Iap2Link* pIap2Link);

#ifdef __cplusplus
}
#endif

#endif


