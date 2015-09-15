
#include "ArchDef.h"
#include "queue.h"

int QUEUE_getContinuousEleCount(Queue* queue)
{
	int count = 0;
	if(queue->m_isEmpty)
	{
		return 0;
	}
	
	if(queue->m_WritePointer > queue->m_ReadPointer)
	{
		count = (queue->m_WritePointer - queue->m_ReadPointer);
	}
	else
	{
		count = (queue->m_nBufferSize - queue->m_ReadPointer);
	}

	if(1 != queue->m_nElementSize)
	{
		count /= queue->m_nElementSize;
	}	

	return count;
}

int QUEUE_getElementCount(Queue* queue)
{
	int count = 0;
	if(queue->m_isEmpty)
	{
		return 0;
	}
	
	if(queue->m_isFull)
	{
		count = queue->m_nBufferSize;
	}
	else if(queue->m_WritePointer > queue->m_ReadPointer)
	{
		count = queue->m_WritePointer - queue->m_ReadPointer;
	}
	else
	{
		count = queue->m_nBufferSize - queue->m_ReadPointer + queue->m_WritePointer;
	}
	
	if(1 != queue->m_nElementSize)
	{
		count /= queue->m_nElementSize;
	}	
	return count;
}

//Return the element count of push out queue 
int QUEUE_PushOutEleArray(Queue* queue, void* pEleArrayBuf, int nMaxEleCount)
{
	int nLen = 0;
	int nCount = 0;
	int i = 0;
	uint8* pData = (uint8*)pEleArrayBuf;
	int nSpace = 0;

	queue->m_isLock = True;
	nSpace = QUEUE_getContinuousEleCount(queue);

	while(nSpace && nMaxEleCount)
	{
		if(nSpace >= nMaxEleCount)
		{
			nLen = nMaxEleCount * queue->m_nElementSize;
			if(pData)
			{
				memcpy(&pData[i], &queue->m_pBuffer[queue->m_ReadPointer], nLen);
			}
			nCount += nMaxEleCount;
			nMaxEleCount -= nSpace;
			
			queue->m_ReadPointer += nLen;
			if(queue->m_ReadPointer >= queue->m_nBufferSize)
			{
				queue->m_ReadPointer = 0;
			}
			queue->m_isFull = False;
			if(queue->m_WritePointer == queue->m_ReadPointer)
			{
				queue->m_isEmpty = True;
			}	
			break;
		}
		else
		{
			nLen = nSpace * queue->m_nElementSize;
			if(pData)
			{
				memcpy(&pData[i], &queue->m_pBuffer[queue->m_ReadPointer], nLen);
			}
			i += nLen;
			nCount += nSpace;
			nMaxEleCount -= nSpace;
			
			queue->m_ReadPointer += nLen;
			if(queue->m_ReadPointer >= queue->m_nBufferSize)
			{
				queue->m_ReadPointer = 0;
			}
			queue->m_isFull = False;
			if(queue->m_WritePointer == queue->m_ReadPointer)
			{
				queue->m_isEmpty = True;
			}	
		}
		nSpace = QUEUE_getContinuousEleCount(queue);
	}
	
	queue->m_isLock = False;
	return nCount;
}

int QUEUE_getContinuousSpace(Queue* queue)
{
	if(queue->m_isEmpty)
	{
		//queue->m_WritePointer = 0;
		//queue->m_ReadPointer = 0;
		return (queue->m_nBufferSize - queue->m_WritePointer) / queue->m_nElementSize;
	}
	else if(queue->m_isFull)
	{
		return 0;
	}
	else if(queue->m_WritePointer > queue->m_ReadPointer)
	{
		return (queue->m_nBufferSize - queue->m_WritePointer) / queue->m_nElementSize;
	}
	else
	{
		return (queue->m_ReadPointer - queue->m_WritePointer) / queue->m_nElementSize;
	}
}

int QUEUE_getSpace(Queue* queue)
{
	if(queue->m_isFull)
	{
		return 0;
	}
	else if(queue->m_isEmpty)
	{
		return queue->m_nBufferSize/queue->m_nElementSize;
	}
	else if(queue->m_WritePointer > queue->m_ReadPointer)
	{
		return (queue->m_nBufferSize - queue->m_WritePointer + queue->m_ReadPointer) / queue->m_nElementSize;
	}
	else
	{
		return (queue->m_ReadPointer - queue->m_WritePointer) / queue->m_nElementSize;
	}
}

//Return the element count of push in queue 
int QUEUE_PushInEleArray(Queue* queue, const void* pEleArray, int nEleCount)
{
	int nLen = 0;
	int nCount = 0;
	int i = 0;
	const uint8* pData = (uint8*)pEleArray;
	int nSpace = QUEUE_getContinuousSpace(queue);

	while(nSpace && nEleCount)
	{
		if(nSpace >= nEleCount)
		{
			nLen = nEleCount * queue->m_nElementSize;
			if(pData)
			{
				memcpy(&queue->m_pBuffer[queue->m_WritePointer], &pData[i], nLen);
			}
			nCount += nEleCount;
			
			queue->m_WritePointer += nLen;
			if(queue->m_WritePointer >= queue->m_nBufferSize)
			{
				queue->m_WritePointer = 0;
			}
			queue->m_isEmpty = False;
			if(queue->m_WritePointer == queue->m_ReadPointer)
			{
				queue->m_isFull = True;
			}	
			break;
		}
		else
		{
			nLen = nSpace * queue->m_nElementSize;
			if(pData)
			{
				memcpy(&queue->m_pBuffer[queue->m_WritePointer], &pData[i], nLen);
			}
			i += nLen;
			nCount += nSpace;
			nEleCount -= nSpace;
			
			queue->m_WritePointer += nLen;
			if(queue->m_WritePointer >= queue->m_nBufferSize)
			{
				queue->m_WritePointer = 0;
			}
			queue->m_isEmpty = False;
			if(queue->m_WritePointer == queue->m_ReadPointer)
			{
				queue->m_isFull = True;
			}
		}
		nSpace = QUEUE_getContinuousSpace(queue);
	}
	
	return nCount;
}

void* QUEUE_getNew(Queue* queue)
{
	void* pElement = &queue->m_pBuffer[queue->m_WritePointer];
	
	if(queue->m_isFull) return Null;
	
	queue->m_WritePointer += queue->m_nElementSize;
	if(queue->m_WritePointer >= queue->m_nBufferSize)
	{
		queue->m_WritePointer = 0;
	}
	
	queue->m_isEmpty = False;
	if(queue->m_WritePointer == queue->m_ReadPointer)
	{
		queue->m_isFull = True;
	}

	return pElement;
}

Bool QUEUE_add(Queue* queue, const void* element, int len)
{
	if(queue->m_isFull || queue->m_nElementSize < len) return False;

	memcpy(&queue->m_pBuffer[queue->m_WritePointer], element, len);

	queue->m_WritePointer += queue->m_nElementSize;
	if(queue->m_WritePointer >= queue->m_nBufferSize)
	{
		queue->m_WritePointer = 0;
	}
	
	queue->m_isEmpty = False;
	if(queue->m_WritePointer == queue->m_ReadPointer)
	{
		queue->m_isFull = True;
	}
	
	return True;
}

void* QUEUE_getHead(Queue* queue)
{
	if(queue->m_isEmpty)
	{
		return Null;
	}
	
	return &queue->m_pBuffer[queue->m_ReadPointer];
}

void* QUEUE_get(Queue* queue, uint32* pNextPos)
{
	void* pRet = Null;

	* pNextPos = 0;
	if(queue->m_isEmpty || INVALID_POS == *pNextPos)
	{
		return Null;
	}

	if(THE_FIRST_POS == *pNextPos)
	{
		*pNextPos = queue->m_ReadPointer;
	}

	Assert((*pNextPos % queue->m_nElementSize) == 0);

	pRet = &queue->m_pBuffer[*pNextPos];
	
	*pNextPos += queue->m_nElementSize;
	if(*pNextPos >= queue->m_nBufferSize)
	{
		*pNextPos = 0;
	}
	
	if(*pNextPos == queue->m_WritePointer)
	{
		*pNextPos = INVALID_POS;
	}
	
	return pRet;
}

void QUEUE_RemoveElements(Queue* queue, int nElements)
{
	QUEUE_PushOutEleArray(queue, Null, nElements);
}

//Pop the head element;
void QUEUE_removeHead(Queue* queue)
{
	queue->m_isLock = True;
	if(queue->m_isEmpty)
	{
		goto End ;
	}
	
	queue->m_ReadPointer += queue->m_nElementSize;
	if(queue->m_ReadPointer >= queue->m_nBufferSize)
	{
		queue->m_ReadPointer = 0;
	}
	
	queue->m_isFull = False;
	if(queue->m_ReadPointer == queue->m_WritePointer)
	{
		queue->m_isEmpty = True;
	}
	
End:
	queue->m_isLock = False;
}

void QUEUE_removeAll(Queue* queue)
{
	queue->m_isLock = True;
	queue->m_ReadPointer = 0;
	queue->m_WritePointer = 0;
	queue->m_isEmpty = True;
	queue->m_isFull = False;
	
	queue->m_isLock = False;
}

//Get the head element and pop it
void* QUEUE_popGetHead(Queue* queue)
{
	void* pvalue = QUEUE_getHead(queue);
	QUEUE_removeHead(queue);
	return pvalue;
}


//Is queue empty 
//return: 1=Empty 0=Not Empty
Bool QUEUE_isEmpty(Queue* queue)
{
	return queue->m_isEmpty;
}

Bool QUEUE_isFull(Queue* queue)
{
	return queue->m_isFull;
}

Bool QUEUE_isLock(Queue* queue)
{
	return queue->m_isLock;
}

Bool QUEUE_init(Queue* queue, void* pBuffer, unsigned short itemSize, uint32 itemCount)
{
	memset(queue, 0, sizeof(Queue));
	queue->m_isUsed = 1;

	queue->m_pBuffer = pBuffer;	
	queue->m_isEmpty = True;
	queue->m_nElementSize = itemSize;
	queue->m_nBufferSize = itemSize * itemCount;

	return True;
}

void QUEUE_reset(Queue* queue)
{
	queue->m_ReadPointer = 0;
	queue->m_WritePointer = 0;
	queue->m_isEmpty = True;
	queue->m_isFull = False;
	memset(queue->m_pBuffer, 0, queue->m_nBufferSize);
}
