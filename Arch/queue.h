

#ifndef __QUEUE_H__
#define __QUEUE_H__    

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

#define INVALID_POS		0xFFFFFFFF
#define THE_FIRST_POS	0xFFFFFFFE

typedef struct _Queue
{
    int m_ReadPointer;
    int m_WritePointer;
	unsigned short	m_nElementSize;		//Element size

    unsigned char m_isUsed:1;
    unsigned char m_isEmpty:1;
    unsigned char m_isFull:1;
    unsigned char m_isLock:1;
    unsigned char m_count:4;
	
	uint32	m_nBufferSize;
    uint8* m_pBuffer;
}Queue;

void* QUEUE_getNew(Queue* queue);
Bool QUEUE_addByte(Queue* queue, uint8 byte);
Bool QUEUE_add(Queue* queue, const void* element, int len);
Queue* QUEUE_create(int siz);

//Get the head element of queue
void* QUEUE_getHead(Queue* queue);
void* QUEUE_get(Queue* queue, uint32* pNextPos);

//Remove the head element;
void QUEUE_removeHead(Queue* queue);

//Get the head element and remove it
void* QUEUE_popGetHead(Queue* queue);

//Is queue empty 
//return: 1=Empty 0=Not Empty
Bool QUEUE_isLock(Queue* queue);
Bool QUEUE_isEmpty(Queue* queue);
Bool QUEUE_isFull(Queue* queue);
void QUEUE_removeAll(Queue* queue);

Bool QUEUE_init(Queue* queue, void* pBuffer, unsigned short itemSize, unsigned int itemCount);
void QUEUE_reset(Queue* queue);
int QUEUE_GetCount(Queue* queue);

int QUEUE_getContinuousEleCount(Queue* queue);
int QUEUE_getElementCount(Queue* queue);
int QUEUE_PushOutEleArray(Queue* queue, void* pEleArrayBuf, int nMaxEleCount);

int QUEUE_getContinuousSpace(Queue* queue);
int QUEUE_getSpace(Queue* queue);
int QUEUE_PushInEleArray(Queue* queue, const void* pEleArray, int nEleCount);
void QUEUE_RemoveElements(Queue* queue, int nElements);

#ifdef __cplusplus
}
#endif

#endif 

