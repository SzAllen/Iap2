
#include "UtpMgr.h"
//#include "wdt.h"
//#include "Usart.h"

#define feed_dog(...)

extern Bool SwTimer_isTimerOut(uint32 initTicks, uint32 newTicks, uint32 timeOutTicks);


#define UtpReq_isTimeOut(pReq)	\
	SwTimer_isTimerOut(pReq->m_dwInitTicks, GET_TICKS(), pReq->m_TimeOutMs)

/*******************************************************************************
* Function Name  : uint8_t check_sum_cal(uint8_t *ptr,uint8_t len)
* Description    : Parity check calculation
* Parameters     : ptr-the for address of the data wanted calculate
                   len-the bytes number of the data wanted calculate
* Return         : the value of the parity check calculation
*******************************************************************************/
uint8 Crc_Get(const void* pData, int len)
{
	uint8 check_sum = 0;
	const uint8 *ptr = (uint8*)pData;

	while (len--)
	{ 
		check_sum ^= *ptr;
		ptr++;
	}
	return check_sum;
}
////////////////////////////////////////////////////////////////////////////////////////////////
void UtpRsp_Reset(UtpRsp* pRsp)
{
	//LOCK();
	pRsp->m_State = USART_INIT;
	//UNLOCK();
}
void UtpRsp_Init(UtpRsp* pRsp, uint8 cmd, ReqProcCb reqProc)
{
	memset(pRsp, 0, sizeof(pRsp));
	
	pRsp->m_Cmd		= cmd;
	pRsp->ReqProc	= reqProc;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void UtpReq_Reset(UtpReq* pReq)
{
	//LOCK();
	pReq->m_TxCounter 	= 0;
	pReq->m_dwInitTicks = 0;
	pReq->m_ErrCode 	= RC_SUCCESS;
	pReq->m_State 		= USART_INIT;
	//UNLOCK();
}

void UtpReq_Init(UtpReq* pReq, uint8 cmd, uint8 maxTxCount, uint32 timeOutMs, RspProcCb rspProc)
{
	memset(pReq, 0, sizeof(UtpReq));
	
	pReq->m_Cmd			= cmd;
	pReq->m_MaxTxCount	= maxTxCount;
	pReq->m_TimeOutMs 	= timeOutMs;
	pReq->RspProc 		= rspProc;

	pReq->m_ErrCode 	= RC_SUCCESS;
	pReq->m_State 		= USART_INIT;
}

////////////////////////////////////////////////////////////////////////////////////////////////

UtpReq* UtpMgr_GetReq(UtpMgr* pUtpMgr, uint8 cmd, uint8 state)
{
	UtpReq* pUtpReq = pUtpMgr->m_pUtpReqArray;
	
	ASRT(pUtpReq);
	
	if(pUtpMgr->m_UtpReqCount == 1)
	{
		if(pUtpReq->m_State == state)
		{
			pUtpReq->m_Cmd = cmd;
			return pUtpReq;
		}
	}
	else
	{
		int i = 0;
		for(i = 0; i < pUtpMgr->m_UtpReqCount; i++, pUtpReq++)
		{
			if(pUtpReq->m_Cmd == cmd && pUtpReq->m_State == state)
			{
				return pUtpReq;
			}
		}
	}
	
	return Null;
}

UtpRsp* UtpMgr_GetRsp(UtpMgr* pUtpMgr, uint8 cmd, uint8 state)
{
	UtpRsp* pUtpRsp = pUtpMgr->m_pUtpRspArray;
	
	ASRT(pUtpRsp);

	if(pUtpMgr->m_UtpRspCount == 1)
	{
		if(pUtpRsp->m_State == state)
		{
			pUtpRsp->m_Cmd = cmd;
			return pUtpRsp;
		}
	}
	else
	{
		int i = 0;
		for(i = 0; i < pUtpMgr->m_UtpRspCount; i++, pUtpRsp++)
		{
			if(pUtpRsp->m_Cmd == cmd && pUtpRsp->m_State == state)
			{
				return pUtpRsp;
			}
		}
	}

	return Null;
}

int UtpMgr_Packet(const uint8* pSrc, int srcLen, uint8* pDst, int dstLen)
{
	int i = 0;

	ASRT(srcLen <= dstLen + 3);

	pDst[i++] = UART_HEAD_KEY;

	memcpy(&pDst[i], pSrc, srcLen);	i += srcLen;
	
	pDst[i++] = Crc_Get(&pSrc[UART_PKT_HEAD_LEN], srcLen - UART_PKT_HEAD_LEN);
	pDst[i++] = UART_TAIL_KEY;

	
	return i;
}

static void UtpMgr_ToBus(UtpMgr* pUtpMgr, const uint8* pData, int len)
{	
	uint8	buff[MAX_UART_PKT_SIZE];

	len = UtpMgr_Packet(pData, len, buff, MAX_UART_PKT_SIZE);

	PF(DL_UTP, ("\n%s: len=%d: ", _FUNC_, len)); 
	DUMP_BYTE_LEVEL(DL_UTP, buff, len);
	
	pUtpMgr->Tx(buff, len);
	
}

static void UtpMgr_ProcRxPkt_Sync(UtpMgr* pUtpMgr, UtpPkt* pPkt)
{
	UtpReq* pUtpReq = &pUtpMgr->m_SyncReq;
	uint8 type = (pPkt->m_Control & 0x03);

	if(UART_DATA_TYPE_RSP == type && pUtpReq->m_ReqPkt.m_Seq == pPkt->m_Seq)
	{
		pUtpReq->m_State 	= USART_RX_RSP;
		pUtpReq->m_ErrCode 	= RC_SUCCESS;
		memcpy(&pUtpReq->m_RspPkt, pPkt, pPkt->m_Len + UART_PKT_HEAD_LEN);
	}
}

void UtpMgr_ProcRxPkt(UtpMgr* pUtpMgr, UtpPkt* pPkt)
{
	int nRet = RC_UNKNOWN;
	UtpRsp* pUtpRsp = Null;
	UtpReq* pUtpReq = Null;
	uint8 type = (pPkt->m_Control & 0x03);
	uint8 nLen = MAX_UART_PAYLOAD_DATA_SIZE;

	if(UART_DATA_TYPE_REQ == type || UART_DATA_TYPE_BOARDCAST == type)
	{
		pUtpRsp = pUtpMgr->GetRsp(pUtpMgr, pPkt->m_Cmd, USART_INIT);
		
		if(pUtpRsp)
		{
			UtpPkt* pReqPkt = &pUtpRsp->m_ReqPkt;
			UtpPkt* pRspPkt = &pUtpRsp->m_RspPkt;
			
			pUtpRsp->m_State 	= USART_RX_REQ;
			memcpy(pReqPkt, pPkt, pPkt->m_Len + UART_PKT_HEAD_LEN);
			
			nRet = pUtpRsp->ReqProc(pUtpMgr->m_pOwner, pReqPkt, pRspPkt->m_Data, &nLen);
			
			if(UART_DATA_TYPE_BOARDCAST == pUtpRsp->m_ReqPkt.m_Control || RC_ABORTED == nRet)
			{
				//No response for boardcast pkt
				UtpRsp_Reset(pUtpRsp);
				return;
			}
			
			if(RC_PENDING == nRet)
			{
				return;
			}
			else if(RC_UNKNOWN == nRet)
			{
				nLen = 1;
 				pRspPkt->m_Data[0] = RC_UNKNOWN;
			}
			
			//Send rsp pkt
			pRspPkt->m_Len 	 = nLen + MAX_PAYLOAD_HEAD_SIZE;
			pRspPkt->m_Control = UART_DATA_TYPE_RSP;
			pRspPkt->m_Cmd 	 = pUtpRsp->m_ReqPkt.m_Cmd;
			pRspPkt->m_Seq 	 = pUtpRsp->m_ReqPkt.m_Seq;
			
			UtpMgr_ToBus(pUtpMgr, (uint8*)&pUtpRsp->m_RspPkt, pRspPkt->m_Len + UART_PKT_HEAD_LEN);
			UtpRsp_Reset(pUtpRsp);
		}
	}
	else if(UART_DATA_TYPE_RSP == type)
	{
		pUtpReq = pUtpMgr->GetReq(pUtpMgr, pPkt->m_Cmd, USART_TX_REQ);

		if(pUtpReq && pUtpReq->m_ReqPkt.m_Seq == pPkt->m_Seq)
		{
			pUtpReq->m_State 	= USART_RX_RSP;
			pUtpReq->m_ErrCode 	= RC_SUCCESS;
			memcpy(&pUtpReq->m_RspPkt, pPkt, pPkt->m_Len + UART_PKT_HEAD_LEN);
			
			if(pUtpReq->RspProc)
			{
				pUtpReq->RspProc(pUtpMgr->m_pOwner, &pUtpReq->m_ReqPkt, pPkt->m_Data, pPkt->m_Len-MAX_PAYLOAD_HEAD_SIZE, pUtpReq->m_ErrCode);
				UtpReq_Reset(pUtpReq);
			}
		}
	}
}

void UtpMgr_Parse(UtpMgr* pUtpMgr, RcvPacketFn fn, const uint8* pData, int len)
{
	int i = 0;
	uint8 cRc = 0;
	if(len == 0 || Null == pData) return;

	for(i = 0; i < len; i++)
	{
		if(pUtpMgr->m_isGetHead)
		{
			if(!pUtpMgr->m_isRxSeq)
			{
				pUtpMgr->m_isRxSeq = True;
				pUtpMgr->m_Buff[pUtpMgr->m_Index++] = pData[i];
				continue;
			}
			if(pUtpMgr->m_Len)
			{
				if(pUtpMgr->m_Index < pUtpMgr->m_Len + UART_PKT_HEAD_LEN)
				{
					pUtpMgr->m_Buff[pUtpMgr->m_Index++] = pData[i];
					continue;
				}
 				else
				{
					//Check CRC
					cRc = Crc_Get(&pUtpMgr->m_Buff[UART_PKT_HEAD_LEN], pUtpMgr->m_Len);
					if(fn) fn(pUtpMgr->m_Buff, pUtpMgr->m_Len + UART_PKT_HEAD_LEN);
					if(cRc == pData[i])
					{
						//Data pkt is valid.
						//UtpMgr_ProcRxPkt(pUtpMgr, (UtpPkt*)pUtpMgr->m_Buff);
						pUtpMgr->ProcRxPkt(pUtpMgr, (UtpPkt*)pUtpMgr->m_Buff);
					}
					else
					{
						PF_WARNING(("CRC[%x] Error[], Discard!", pData[i], cRc));
						DUMP_BYTE_LEVEL(DL_WARNING, pUtpMgr->m_Buff, pUtpMgr->m_Len + UART_PKT_HEAD_LEN);
						//Data pkt is invalid, discard 
					}

					UtpMgr_Reset(pUtpMgr);
				}
			}
			else
			{
				if(pData[i] > MAX_UART_PAYLOAC_SIZE || pData[i] < MAX_PAYLOAD_HEAD_SIZE)
				{
					PF_WARNING(("Uart Pkt Len[%d] is Error, Discard.\n", pData[i]));
					if(fn) fn(pUtpMgr->m_Buff, pUtpMgr->m_Len);
					UtpMgr_Reset(pUtpMgr);
					continue;
				}
				pUtpMgr->m_Len = pData[i];
				pUtpMgr->m_Buff[pUtpMgr->m_Index++] = pData[i];
			}
		}
		else if(UART_HEAD_KEY == pData[i])
		{
			pUtpMgr->m_isGetHead = True;
			pUtpMgr->m_Index 	= 0;
			pUtpMgr->m_Len 		= 0;
			pUtpMgr->m_isRxSeq 	= 0;
		}	
	}
}

static void UtpMgr_CheckRxBuf(UtpMgr* pUtpMgr)
{
#ifdef WIN32
#else
	uint8 buff[10];
	int i = USART_Read(pUtpMgr->m_DriverId, buff, sizeof(buff));
	if(i)
	{
		//DUMP_BYTE_LEVEL(DL_UTP, buff, i);
		UtpMgr_Parse(pUtpMgr, buff, i);
	}
#endif
}

void UtpMgr_Reset(UtpMgr* pUtpMgr)
{
	pUtpMgr->m_isGetHead = 0;
	pUtpMgr->m_Index 	= 0;
	pUtpMgr->m_Len 		= 0;
}

static void UtpMgr_TxPkt(UtpMgr* pUtpMgr, UtpReq* pUtpReq)
{
	pUtpReq->m_TxCounter++;
	pUtpReq->m_dwInitTicks= GET_TICKS();
	UtpMgr_ToBus(pUtpMgr, (uint8*)&pUtpReq->m_ReqPkt, pUtpReq->m_ReqPkt.m_Len + 2);
}

static uint8 UtpMgr_Post(UtpMgr* pUtpMgr, UtpReq* pUtpReq)
{
//	Bool bRet = False;
	uint8 type = pUtpReq->m_ReqPkt.m_Control && 0x03;
	
	//LOCK();
	if(UART_DATA_TYPE_REQ  == type)
	{
		pUtpReq->m_State 	= USART_TX_REQ;
	}
	else if(UART_DATA_TYPE_RSP  == type)
	{
		pUtpReq->m_State 	= USART_TX_RSP;
	}

	if(0 == ++pUtpMgr->m_Seq) pUtpMgr->m_Seq = 1;
	pUtpReq->m_ReqPkt.m_Seq = pUtpMgr->m_Seq;

	UtpMgr_TxPkt(pUtpMgr, pUtpReq);
	
	if(UART_DATA_TYPE_BOARDCAST == pUtpReq->m_ReqPkt.m_Control)
	{
		UtpReq_Reset(pUtpReq);
	}
	//UNLOCK();
	
	return pUtpReq->m_ReqPkt.m_Seq;
}

static uint8 UtpMgr_Send(UtpMgr* pUtpMgr, UtpReq* pUtpReq, uint8 maxReTxCounter)
{	
	//Get pUtpMgr->ProcRxPkt, restore the function pointer
	UtpRxFun procRxPkt = pUtpMgr->ProcRxPkt;

	//Change the function pointer pUtpMgr->ProcRxPkt
	pUtpMgr->ProcRxPkt = UtpMgr_ProcRxPkt_Sync;
	
	UtpMgr_Post(pUtpMgr, pUtpReq);
	while(pUtpReq->m_State == USART_TX_REQ && (pUtpReq->m_TxCounter < maxReTxCounter || maxReTxCounter == MAX_RETX_COUNTER))
	{
		UtpMgr_CheckRxBuf(pUtpMgr);
		
		if(SwTimer_isTimerOut(pUtpReq->m_dwInitTicks, GET_TICKS(), pUtpReq->m_TimeOutMs))
		{
			feed_dog();
			UtpMgr_TxPkt(pUtpMgr, pUtpReq);
		}
	}

	// restore the function pointer
	pUtpMgr->ProcRxPkt = procRxPkt;

	if(USART_RX_RSP == pUtpReq->m_State && RC_SUCCESS == pUtpReq->m_ErrCode)
	{
		return pUtpMgr->m_Seq;
	}

	return 0;
}

uint8 UtpMgr_PostReq(UtpMgr* pUtpMgr, uint8 cmd, const uint8* pData, int len, int nReTxCount)
{
	uint8 seq = 0;
	UtpReq* pUtpReq = pUtpMgr->GetReq(pUtpMgr, cmd, USART_INIT);
	
	if(pUtpReq)
	{
		UtpPkt* pPkt = &pUtpReq->m_ReqPkt;

		//PF(DL_UTP, ("%s, cmd=0x%x, len=%d\n", _FUNC_, cmd, len));
		
		pUtpReq->m_MaxTxCount = nReTxCount;
		pPkt->m_Control = UART_DATA_TYPE_REQ;
		pPkt->m_Cmd 	= cmd;
		ASRT(len <= MAX_UART_PAYLOAD_DATA_SIZE);
		if(len > 0)
		{
			memcpy(pPkt->m_Data, pData, len);
		}
		pPkt->m_Len = len + MAX_PAYLOAD_HEAD_SIZE;
		
		seq = UtpMgr_Post(pUtpMgr, pUtpReq);
	}

	return seq;
}


Bool UtpMgr_SendReq(UtpMgr* pUtpMgr, uint8 cmd, const void* pData, int len, uint8* pRspData, int* pRcvlen, uint8 maxReTxCounter)
{
	Bool bRet = False;
	UtpReq* pUtpReq = &pUtpMgr->m_SyncReq;
	UtpPkt* pPkt = &pUtpReq->m_ReqPkt;

	memset(pUtpReq, 0, sizeof(UtpReq));
	
	pUtpReq->m_Cmd			= cmd;
	pUtpReq->m_MaxTxCount 	= maxReTxCounter;
	pUtpReq->m_TimeOutMs	= DEFAULT_MS;
	pUtpReq->m_State		= USART_INIT;
	
	ASRT(len <= MAX_UART_PAYLOAD_DATA_SIZE);

	*pRcvlen = 0;
	
	pPkt->m_Control = UART_DATA_TYPE_REQ;
	pPkt->m_Cmd 	= cmd;
	memcpy(pPkt->m_Data, pData, len);
	pPkt->m_Len = len + MAX_PAYLOAD_HEAD_SIZE;
	
	if(UtpMgr_Send(pUtpMgr, pUtpReq, maxReTxCounter))
	{
		if(pRspData && *pRcvlen <= pUtpReq->m_RspPkt.m_Len - MAX_PAYLOAD_HEAD_SIZE)
		{
			*pRcvlen = pUtpReq->m_RspPkt.m_Len - MAX_PAYLOAD_HEAD_SIZE;
			memcpy(pRspData, pUtpReq->m_RspPkt.m_Data, * pRcvlen);
			bRet = True;
		}
	}

	return bRet;
}

void UtpMgr_Run(UtpMgr* pUtpMgr)
{
	int i = 0;
	UtpReq* pUtpReq = pUtpMgr->m_pUtpReqArray;

	UtpMgr_CheckRxBuf(pUtpMgr);

	//Check if received req pkt.
	for(i = 0; pUtpReq && i < pUtpMgr->m_UtpReqCount; i++, pUtpReq++)
	{
		if(USART_TX_REQ == pUtpReq->m_State)
		{
			if(UtpReq_isTimeOut(pUtpReq))
			{
				if(pUtpReq->m_TxCounter < pUtpReq->m_MaxTxCount)
				{
					//Re send req pkt.
					Printf("ReTx %d. ", pUtpReq->m_TxCounter);
					UtpMgr_TxPkt(pUtpMgr, pUtpReq);
				}
				else
				{
					//Notify the caller with sending req failed.
					pUtpReq->m_ErrCode = RC_TIME_OUT;
					pUtpReq->RspProc(pUtpMgr->m_pOwner, &pUtpReq->m_ReqPkt, Null, 0, pUtpReq->m_ErrCode);
					UtpReq_Reset(pUtpReq);
				}
			}
		}
	}
	
}

void UtpMgr_Init(UtpMgr* pUtpMgr
	, UtpTxFun	TxCb
	, ReqProcCb reqProcCb
	, RspProcCb rspProcCb
	, void* 	pOnwer
	, uint8		driverId
	)
{

	memset(pUtpMgr, 0, sizeof(UtpMgr));

	pUtpMgr->m_DriverId = driverId;

	UtpRsp_Init(&pUtpMgr->m_UtpRsp, 0, reqProcCb);
	UtpReq_Init(&pUtpMgr->m_UtpReq, 0, 2, DEFAULT_RESEND_INTERVAL, rspProcCb);
	
	pUtpMgr->m_pUtpRspArray = &pUtpMgr->m_UtpRsp;
	pUtpMgr->m_UtpRspCount  = 1;
	
	pUtpMgr->m_pUtpReqArray = &pUtpMgr->m_UtpReq;
	pUtpMgr->m_UtpReqCount  = 1;

	pUtpMgr->Tx 		= TxCb;
	pUtpMgr->ProcRxPkt  = UtpMgr_ProcRxPkt;
	
	pUtpMgr->GetReq = UtpMgr_GetReq;
	pUtpMgr->GetRsp = UtpMgr_GetRsp;

	pUtpMgr->m_pOwner = pOnwer;
}

/*
void UtpMgr_Init(UtpMgr* pUtpMgr
	, UtpTxFun	TxCb
	, UtpRsp* pUtpRspArray
	, int 	  nUtpRspCount
	, UtpReq* pUtpReqArray
	, int 	  nUtpReqCount
	, void* 	pOnwer
	)
{
	memset(pUtpMgr, 0, sizeof(UtpMgr));

	pUtpMgr->m_pUtpRspArray 	= pUtpRspArray;
	pUtpMgr->m_UtpRspCount  = nUtpRspCount;
	
	pUtpMgr->m_pUtpReqArray = pUtpReqArray;
	pUtpMgr->m_UtpReqCount  = nUtpReqCount;

	pUtpMgr->Tx = TxCb;
	pUtpMgr->Rx = UtpMgr_ProcRxPkt;
	
	pUtpMgr->GetReq = UtpMgr_GetReq;
	pUtpMgr->GetRsp = UtpMgr_GetRsp;

	pUtpMgr->m_pOwner = pOnwer;
}
*/
