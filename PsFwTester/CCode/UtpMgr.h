#ifndef __UTP_MGR__H_
#define __UTP_MGR__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "Archdef.h"

#define UTP_RC_SUCCESS		0
#define UTP_RC_FAILED		1
#define UTP_RC_NOT_READY	2
#define UTP_RC_UNSUPPORTED	0XFF

#define DEFAULT_RESEND_INTERVAL 500

#define MAX_RETX_COUNTER			0xFF
#define MAX_PENDING_RX_PKT_COUNT 	10
#define MAX_PENDING_TX_PKT_COUNT 	10

#define MIN_PKT_LEN					7
#define MAX_UART_PURE_DATA_SIZE		128
#define MAX_UART_PAYLOAD_DATA_SIZE	(MAX_UART_PURE_DATA_SIZE+4)
#define MAX_UART_PAYLOAC_SIZE 		(MAX_UART_PAYLOAD_DATA_SIZE + 2)	//Not include the pkt head/tail/crc/len
#define MAX_UART_PKT_SIZE 			(MAX_UART_PAYLOAC_SIZE + 5)			//include the pkt head/tail/crc/len

#define UART_HEAD_KEY	0X55
#define UART_TAIL_KEY	0XAA

#define UART_DATA_TYPE_BOARDCAST 	0x00
#define UART_DATA_TYPE_REQ 			0x01
#define UART_DATA_TYPE_RSP 			0x02

#define DEFAULT_SYNC_TIME_OUT_MS	100

#define UART_PKT_HEAD_LEN			2	
#define MAX_PAYLOAD_HEAD_SIZE 		2	
	
#define UTP_IS_FORWORD(control) (control & 0x04)
#define UTP_ISREQ(control) ((control & 0x03) == 1)
#define UTP_ISRSP(control) ((control & 0x03) == 2)

#define DEFAULT_MS		500
#define RE_TX_COUNT		8

typedef struct _UtpPkt
{
	//Uart pkt head len
	uint8	m_Seq;
	uint8	m_Len;

	//Payload
	uint8	m_Control;	
	uint8	m_Cmd;
	uint8	m_Data[MAX_UART_PAYLOAD_DATA_SIZE];
}UtpPkt;
#define MIN_UART_PKT_SIZE	(sizeof(UtpPkt) - MAX_UART_PAYLOAD_DATA_SIZE)

////////////////////////////////////////////////////////////////////////////////////////////////
enum
{
	 USART_INIT = 0
	,USART_TX_REQ
	,USART_RX_RSP
	,USART_RX_REQ
	,USART_TX_RSP
};

typedef void (*RspProcCb)(void* pArg, UtpPkt* pUtpPkt, uint8* pRspData, uint8 rspDataLen, uint8 state);
typedef int (*ReqProcCb)(void* pArg, UtpPkt* pUtpPkt, uint8* pRspData, uint8* pRspDataLen);

typedef struct _UtpReq
{
	uint8	m_Cmd;
	uint8	m_MaxTxCount;
	uint32	m_TimeOutMs;
	
	RspProcCb	RspProc;
	
	UtpPkt m_ReqPkt;
	UtpPkt m_RspPkt;
	
	volatile uint8	m_State;
	uint8	m_ErrCode;
	uint8	m_TxCounter;
		
	uint32	m_dwInitTicks;
}UtpReq;

void UtpReq_Init(UtpReq* pReq, uint8 cmd, uint8 maxTxCount, uint32 timeOutMs, RspProcCb rspProc);
void UtpReq_Reset(UtpReq* pRsp);
Bool UtpReq_isTimeOut(UtpReq* pReq);

////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _UtpRsp
{
	uint8		m_Cmd;
	ReqProcCb	ReqProc;
	
	UtpPkt 		m_ReqPkt;
	UtpPkt 		m_RspPkt;
	volatile uint8	m_State;
}UtpRsp;

void UtpRsp_Reset(UtpRsp* pRsp);
void UtpRsp_Init(UtpRsp* pRsp, uint8 cmd, ReqProcCb reqProc);

////////////////////////////////////////////////////////////////////////////////////////////////
struct _UtpMgr;
typedef int (*UtpTxFun)(const uint8* pData, int DataLen);
typedef void (*UtpRxFun)(struct _UtpMgr* pUtpMgr, UtpPkt* pRspData);
typedef UtpReq* (*GetUtpReqFun)(struct _UtpMgr* pUtpMgr, uint8 cmd, uint8 state);
typedef UtpRsp* (*GetUtpRspFun)(struct _UtpMgr* pUtpMgr, uint8 cmd, uint8 state);
#define   MAX_USART_RAWBUF_SIZE		MAX_UART_PKT_SIZE
typedef struct _UtpMgr
{
	uint8	m_isGetHead:1;
	uint8	m_isRxSeq:1;
	uint8	m_Reserved:6;

	uint8	m_DriverId;
	uint8	m_Version;
	
	uint8	m_Seq;
	
	uint8	m_Index;
	uint8	m_Len;
	uint8	m_Buff[MAX_UART_PKT_SIZE];

	uint8	m_RxBufInd:1;
	uint8	m_IsSwitch:1;
	uint8	m_Reserved1:6;
	
	int		m_RawLen[2];
	uint8	m_RawBuff[2][MAX_USART_RAWBUF_SIZE];
	
	UtpRsp 	m_UtpRsp;
	UtpReq 	m_UtpReq;
	UtpReq 	m_SyncReq;
	
	UtpRsp* m_pUtpRspArray;
	int 	m_UtpRspCount;
	
	UtpReq* m_pUtpReqArray;
	int 	m_UtpReqCount;

	//Function define.
	UtpTxFun	Tx;
	UtpRxFun	ProcRxPkt;
	
	GetUtpReqFun	GetReq;
	GetUtpRspFun	GetRsp;

	void* m_pOwner;
}UtpMgr;

void UtpMgr_Reset(UtpMgr* pUtpMgr);
void UtpMgr_Init(UtpMgr* pUtpMgr
	, UtpTxFun	TxCb
	, ReqProcCb reqProcCb
	, RspProcCb rspProcCb
	, void* 	pOnwer
	, uint8		driverId	//UsartId
	);

Bool UtpMgr_PostReq(UtpMgr* pUtpMgr, uint8 cmd, const uint8* pData, int len, int nReTxCount);
Bool UtpMgr_SendReq(UtpMgr* pUtpMgr, uint8 cmd, const void* pData, int len, uint8* pRspData, int* pRcvlen, uint8 maxReTxCounter);

typedef void (*RcvPacketFn)(const uint8* pData, int len);
void UtpMgr_Run(UtpMgr* pUtpMgr);
void UtpMgr_Parse(UtpMgr* pUtpMgr, RcvPacketFn fn, const uint8* pData, int len);
void UtpMgr_ProcRxPkt(UtpMgr* pUtpMgr, UtpPkt* pPkt);

int 	UtpMgr_Packet(const uint8* pSrc, int srcLen, uint8* pDst, int dstLen);
UtpReq* UtpMgr_GetReq(UtpMgr* pUtpMgr, uint8 cmd, uint8 state);
UtpRsp* UtpMgr_GetRsp(UtpMgr* pUtpMgr, uint8 cmd, uint8 state);

#ifdef __cplusplus
}
#endif

#endif


