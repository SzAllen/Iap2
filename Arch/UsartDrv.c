
#include "UsartDrv.h"

void UsartDrv_Init(UsartDrv* pUsartDrv
	, uint8 usartId
	, uint32 clk
	, uint32 pIo
	, uint8 rxPin
	, uint8 txPin
	, RecvDataFun recvFn
	, void* pObj)
{	
	memset(pUsartDrv, 0, sizeof(UsartDrv));
	
	pUsartDrv->m_UsartId  	= usartId;
	pUsartDrv->m_pHandleObj	= pObj;
	pUsartDrv->UsartRecvCb 	= recvFn;
	
	pUsartDrv->m_RxPin		= rxPin;
	pUsartDrv->m_TxPin		= txPin;
	
	if(0 == usartId)
	{
		pUsartDrv->m_pUsart = USART0;
		pUsartDrv->m_IrqType= USART0_IRQn;
	}
	else if(1 == usartId)
	{
		pUsartDrv->m_pUsart = USART1;
		pUsartDrv->m_IrqType= USART1_IRQn;
	}
	else if(2 == usartId)
	{
		pUsartDrv->m_pUsart = USART2;
		pUsartDrv->m_IrqType= USART2_IRQn;
	}
	
    pio_configure_pin(rxPin, pIo);
    pio_configure_pin(txPin, pIo);
	
    pUsartDrv->m_pPdc = usart_get_pdc_base(pUsartDrv->m_pUsart);

	UsartDrv_SetClk(pUsartDrv, clk);
	UsartDrv_Disable(pUsartDrv);
}

void UsartDrv_SetClk(UsartDrv* pUsartDrv, uint32 clk)
{
    const sam_usart_opt_t usart_console_settings = {
        BOARD_USART_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		//This field is only used in IrDA mode.
		0
    };

    //Enable the peripheral clock in the PMC.
    pmc_enable_periph_clk(pUsartDrv->m_pUsart);
	
    //Configure USART in serial mode.
    usart_init_rs232(pUsartDrv->m_pUsart, &usart_console_settings, clk);
}

void UsartDrv_Disable(UsartDrv* pUsartDrv)
{
    pdc_disable_transfer(pUsartDrv->m_pPdc, PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS);
	
    usart_disable_interrupt(pUsartDrv->m_pUsart, ALL_INTERRUPT_MASK);
	
    usart_reset_status(pUsartDrv->m_pUsart);
    
    usart_disable_rx(pUsartDrv->m_pUsart);
    usart_disable_tx(pUsartDrv->m_pUsart);
    
    usart_reset_rx(pUsartDrv->m_pUsart);
    usart_reset_tx(pUsartDrv->m_pUsart);	
}

void UsartDrv_Enable(UsartDrv* pUsartDrv)
{
    // Enable the receiver and transmitter.
    pdc_enable_transfer(pUsartDrv->m_pPdc, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);
	
    usart_enable_tx(pUsartDrv->m_pUsart);
    usart_enable_rx(pUsartDrv->m_pUsart);
	
    pUsartDrv->m_RxPdcPkt.ul_addr = (uint32)pUsartDrv->m_RawBuff;
    pUsartDrv->m_RxPdcPkt.ul_size = pUsartDrv->m_RawLen;
    pdc_rx_init(pUsartDrv->m_pPdc, &pUsartDrv->m_RxPdcPkt, Null);
}

void UsartDrv_Tx(UsartDrv* pUsartDrv, const void* pData, int len)
{
    pUsartDrv->m_TxPdcPkt.ul_addr = (uint32)pData;
    pUsartDrv->m_TxPdcPkt.ul_size = len;
    pdc_tx_init(pUsartDrv->m_pPdc, &pUsartDrv->m_TxPdcPkt, Null);
	
}

