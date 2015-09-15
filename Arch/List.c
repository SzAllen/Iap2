
#include "ArchDef.h"
#include "List.h"

void List_Init(List* pList)
{
	memset(pList, 0, sizeof(List));
}

List* List_AddTail(List* pHead, List* pNode)
{
	List* pListNode = pHead;

	ASRT(pNode);
	
	pNode->m_pNext = Null;
	if(pHead == Null)
	{
		pNode->m_pPre = Null;
		pHead = pNode;
		goto END;
	}

	//Get the tail node
	while(pListNode->m_pNext)
	{
		pListNode = pListNode->m_pNext;
		ASRT(pListNode != pNode);
	}

	pListNode->m_pNext = pNode;
	
	pNode->m_pPre = pListNode;
END:
	PF(DL_LISTEX, ("List_AddTail(), count=%d\n", List_Count(pHead)));
	return pHead;
}

void List_RemoveAll(List* pNode)
{
	List* pTemp = pNode;

	while(pTemp)
	{
		pTemp = List_Remove(pNode);
	}
}

//Remove a node form list
//Return: head node
List* List_Remove(List* pNode)
{
	List* pHead = pNode;
	
	ASRT(pNode);

	//Head of list
	if(pNode->m_pPre == Null)
	{
		pHead = pNode->m_pNext;
		if(pHead)
		{
			pHead->m_pPre = Null;
		}
		goto END;
	}
	else
	{
		pNode->m_pPre->m_pNext = pNode->m_pNext;
		
		if(pNode->m_pNext)
		{
			pNode->m_pNext->m_pPre = pNode->m_pPre;
		}
	}

	while(pHead->m_pPre)
	{
		pHead = pHead->m_pPre;
	}

END:	
	pNode->m_pPre = Null;
	pNode->m_pNext = Null;
	
	PF(DL_LISTEX, ("List_Remove(), count=%d\n", List_Count(pHead)));
	return pHead;
}

Bool List_isIn(List* pHead, List* pNode)
{
	if(Null == pHead || Null == pNode)
	{
		return False;
	}

	if(pHead == pNode)
	{
		return True;
	}
	
	pHead = pHead->m_pNext;
	while(pHead)
	{
		if(pHead == pNode)
		{
			return True;
		}

		pHead = pHead->m_pNext;
	}

	return False;
}

int List_Count(List* pNode)
{
	List* pHead = pNode;
	int nCount = 0;

	if(pNode == Null)
	{
		return 0;
	}

	while(pHead)
	{
		pHead = pHead->m_pPre;
		nCount++;
	}

	pNode = pNode->m_pNext;
	while(pNode)
	{
		pNode = pNode->m_pNext;
		nCount++;
	}	

	return nCount;
}

