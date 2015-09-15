#ifdef CONFIG_QUEUEEX

#include "QueueEx.h"
#include "Sd_mmc.h"
#include "Nvd.h"
#include "Media.h"

#define QPF(...)	//PF
#define QDUMP_BYTE_LEVEL(...) //DUMP_BYTE_LEVEL

static QueueEx g_QueueEx;
QueueEx* g_pQueueEx = &g_QueueEx;

static uint8 g_QueueExBuff[SECTOR_SIZE];
extern uint8_t msc_bulk_buffer[PINGPONG_COUNT][PINGPONG_BUFFER_SIZE];
#define QUEUE_EX_ASRT(parenExpr) if(!(parenExpr))	\
		{                                   \
			while(1){;}	\
		}

int 		  g_DebugInfoLen = 0;
static uint8* g_pDebugInfoBuf = &msc_bulk_buffer[0][0];

#define QASRT(parenExpr) if(!(parenExpr))	\
		{                                   \
			while(1){;}	\
		}

int QueueEx_Read(QueueEx * pQueueEx, uint8* pData, int len)
{
	Queue* queue = (Queue*)pQueueEx;
	int nReadLen 	= 0;
	int nIndex 		= 0;
	int nCopyLen	= 0;
	int 	readLba = 0;
	int 	writeLba = queue->m_WritePointer / SECTOR_SIZE;
	
	while(nReadLen < len && (!QUEUE_isEmpty(queue)))
	{
		readLba = queue->m_ReadPointer / SECTOR_SIZE;

		nIndex = queue->m_ReadPointer % SECTOR_SIZE;
		if(writeLba != readLba || (queue->m_ReadPointer > queue->m_WritePointer))
		{
			nCopyLen = (len > (SECTOR_SIZE - nIndex)) ? (SECTOR_SIZE - nIndex) : len;
			sd_read_blk(g_TempBuff, readLba + NVD_LOG_DATA_LBA);
			memcpy(&pData[nReadLen], &g_TempBuff[nIndex], nCopyLen);
		}
		else
		{
			nCopyLen = (len > queue->m_WritePointer - queue->m_ReadPointer) ? (queue->m_WritePointer - queue->m_ReadPointer) : len;
			memcpy(&pData[nReadLen], &g_QueueExBuff[nIndex], nCopyLen);
		}
		nReadLen += nCopyLen;
		QUEUE_RemoveElements(queue, nCopyLen);		
	}
		
	return nReadLen;
}


static int QueueEx_Write(QueueEx * pQueueEx, const void* pData, int len)
{
//	Queue* queue = (Queue*)pQueueEx;
	const uint8* pBytes = (uint8*)pData;
	int writeLba 	= pQueueEx->m_WritePos / SECTOR_SIZE + NVD_LOG_DATA_LBA;
	int nIndex 		= pQueueEx->m_WritePos % SECTOR_SIZE;
	
	QPF(DL_MAIN, ("%s(), pdata=0x%x, len=%d, lba=%d, nIndex=%d\n", _FUNC_, pData, len, writeLba, nIndex));
	if(Null == pData || len == 0) return 0;

	QUEUE_EX_ASRT(nIndex + len <= SECTOR_SIZE);

	if(nIndex)
	{
		sd_read_blk(g_TempBuff, writeLba);
	}

	memcpy(&g_TempBuff[nIndex], &pBytes[nIndex], len);
	sd_write_blk(g_TempBuff, writeLba);

	QDUMP_BYTE_LEVEL(DL_MAIN, g_TempBuff, len);
	
	return True;
}

void QueueEx_Flush()
{
	QueueEx * pQueueEx = &g_QueueEx;
	Queue* queue = (Queue*)pQueueEx;
	int nIndex 	 = pQueueEx->m_WritePos % SECTOR_SIZE;
	
	if(!sd_initialized) return;

	QUEUE_EX_ASRT(pQueueEx->m_WriteLen <= SECTOR_SIZE);
	
	if(pQueueEx->m_WriteLen)
	{
		QueueEx_Write(pQueueEx, &g_TempBuff[nIndex], pQueueEx->m_WriteLen);
		pQueueEx->m_WritePos = queue->m_WritePointer;
		pQueueEx->m_WriteLen = 0;

		QueueEx_Serialize(True);
	}
}

static Bool QueueEx_PushInEx(const void* element, int len)
{
	QueueEx * pQueueEx = &g_QueueEx;
	Queue* queue = (Queue*)pQueueEx;
	int nProcLen = 0;
	int nResidue = len;
	const uint8* pBytes = (uint8*)element;
	int occupyLen = 0;
 	
	QPF(DL_MAIN, ("%s(), PushInLen=%d\n"
		, _FUNC_, len));
	
	while(nResidue)
	{
		if(QUEUE_isFull(queue))
		{
			QUEUE_RemoveElements(queue, SECTOR_SIZE);
		}
		
		occupyLen = QUEUE_getElementCount(queue);
		occupyLen %= SECTOR_SIZE;

		if(occupyLen + nResidue >= SECTOR_SIZE)
		{
			nProcLen = SECTOR_SIZE - occupyLen;
		}
		else if(nResidue > SECTOR_SIZE)
		{
			nProcLen = SECTOR_SIZE;
		}
		else if(pQueueEx->m_WriteLen + nResidue >= SECTOR_SIZE)
		{
			nProcLen = SECTOR_SIZE - pQueueEx->m_WriteLen;
		}
		else
		{
			nProcLen = nResidue;
		}

		QASRT(pQueueEx->m_WriteLen == queue->m_WritePointer % SECTOR_SIZE);

		QUEUE_PushInEleArray(queue, Null, nProcLen);
		memcpy(&g_QueueExBuff[pQueueEx->m_WriteLen], &pBytes[len-nResidue], nProcLen);
		pQueueEx->m_WriteLen += nProcLen;

		QPF(DL_MAIN, ("%s(), occupyLen=%d, nProcLen=%d, nResidue=%d, WritePos=%d, WriteLen=%d\n"
			, _FUNC_, occupyLen, nProcLen, nResidue, pQueueEx->m_WritePos, pQueueEx->m_WriteLen));

		if(SECTOR_SIZE == pQueueEx->m_WriteLen)
		{
			QueueEx_Write(pQueueEx, g_QueueExBuff, pQueueEx->m_WriteLen);
			
			pQueueEx->m_WritePos = queue->m_WritePointer;
			pQueueEx->m_WriteLen = 0;
		}
		nResidue -= nProcLen;
	}

	return True;
}

Bool QueueEx_PushIn(const void* element, int len)
{
	QueueEx * pQueueEx = &g_QueueEx;
	if(!sd_initialized || QUEUEEX_INIT == pQueueEx->m_State)
	{
		if(QUEUEEX_START != pQueueEx->m_State)
		{
			if(g_DebugInfoLen < 5*1024)
			{
				memcpy(&g_pDebugInfoBuf[g_DebugInfoLen], element, len);
				g_DebugInfoLen += len;
			}
			return True;
		}
		return False;
	}
	else if(g_DebugInfoLen)
	{
		QueueEx_PushInEx(g_pDebugInfoBuf, g_DebugInfoLen);
		g_DebugInfoLen = 0;
	}

	return QueueEx_PushInEx(element, len);
}

int QueueEx_GetElememts(QueueEx* pQueueEx, int offset, uint8* pBuff, int len)
{
	int getLen = 0;
	Queue* queue = (Queue*)pQueueEx;
	int nReadPointer = queue->m_ReadPointer;
	
	QUEUE_RemoveElements(queue, offset);

	getLen = QueueEx_Read(pQueueEx, pBuff, len);

	queue->m_ReadPointer = nReadPointer;

	return getLen;
}

int QueueEx_GetBytesCount(QueueEx* pQueueEx)
{
	return QUEUE_getElementCount(&pQueueEx->m_Queue);
}

void QueueEx_Reset()
{
	QueueEx * pQueueEx = &g_QueueEx;
	QUEUE_init(&pQueueEx->m_Queue, g_QueueExBuff, 1, MAX_DEBUG_INFO_QUEUE_BUF_SIZE);
	pQueueEx->m_WriteLen = 0;
	pQueueEx->m_WritePos = 0;
}

void QueueEx_Init(QueueEx* pQueueEx)
{
	memset(pQueueEx, 0, sizeof(QueueEx));

	QUEUE_init(&pQueueEx->m_Queue, g_QueueExBuff, 1, MAX_DEBUG_INFO_QUEUE_BUF_SIZE);
	pQueueEx->m_State = QUEUEEX_START;
	
	QPF(DL_MAIN, ("%s()\n", _FUNC_));
}

void QueueEx_Serialize(Bool isWrite)
{
	QueueEx * pQueueEx = &g_QueueEx;
	uint8 	nvd[SECTOR_SIZE];

	memset(nvd, 0, SECTOR_SIZE);
	
	if(isWrite)
	{
 		memcpy(nvd, pQueueEx, sizeof(QueueEx));
  		Nvd_WriteOneSector(nvd, NVD_LOG_LBA);
	}
	else
	{
   		if(Nvd_ReadOneSector(nvd, NVD_LOG_LBA))
   		{
	 		memcpy(pQueueEx, nvd, sizeof(QueueEx));
			pQueueEx->m_Queue.m_nBufferSize = MAX_DEBUG_INFO_QUEUE_BUF_SIZE;
			pQueueEx->m_State = QUEUEEX_START;
   		}
		else
		{
			QueueEx_Init(pQueueEx);
		}
		
		if(g_DebugInfoLen)
		{
			QueueEx_PushIn(g_pDebugInfoBuf, g_DebugInfoLen);
		}
	}
}


#endif

