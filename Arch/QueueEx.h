#ifndef __DEBUG_INFO__H_
#define __DEBUG_INFO__H_

#ifdef __cplusplus
extern "C"{
#endif

#ifdef CONFIG_QUEUEEX

#include "ArchDef.h"

#define QUEUEEX_INIT 	0
#define QUEUEEX_START 	1

#define MAX_DEBUG_INFO_QUEUE_BUF_SIZE (1024*1024)
typedef struct _QueueEx
{
	Queue m_Queue;

	uint32	m_WritePos;
	uint16	m_WriteLen;
	uint8	m_State;
}QueueEx;
extern QueueEx* g_pQueueEx;

void QueueEx_Init(QueueEx* pQueueEx);
void QueueEx_Reset();
int QueueEx_GetBytesCount(QueueEx* pQueueEx);
int QueueEx_GetElememts(QueueEx* pQueueEx, int offset, uint8* pBuff, int len);
Bool QueueEx_PushIn(const void* element, int len);
void QueueEx_Flush();
void QueueEx_Serialize(Bool isWrite);
int QueueEx_Read(QueueEx * pQueueEx, uint8* pData, int len);

#endif

#ifdef __cplusplus
}
#endif

#endif

