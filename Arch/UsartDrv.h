#ifndef __USART_DRV_H_
#define __USART_DRV_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "Typedef.h"
#include "Usart.h"
#include "pdc.h"
#include "UtpMgr.h"

typedef void (*RecvDataFun)(void* pObj, uint8* pData, uint16 len);

#define   MAX_USART_RAWBUF_SIZE		MAX_UART_PKT_SIZE
typedef struct _UsartDrv
{
	uint8 	m_UsartId;
	uint8	m_RxPin;
	uint8	m_TxPin;
	
	Usart*	m_pUsart;
	Pdc *	m_pPdc;
	
	volatile int	m_RawLen;
	uint8			m_RawBuff[MAX_USART_RAWBUF_SIZE];
	
	volatile int	m_TxLen;
	uint8			m_TxBuff[MAX_USART_RAWBUF_SIZE];

	pdc_packet_t	m_RxPdcPkt;
	pdc_packet_t	m_TxPdcPkt;
	
	RecvDataFun UsartRecvCb;
	void*		m_pHandleObj;
	IRQn_Type	m_IrqType;
}UsartDrv;

void UsartDrv_Init(uint8 usartId, uint8 rxPin, uint8 txPin, RecvDataFun recvFn, void* pObj);

#ifdef __cplusplus
}
#endif

#endif


