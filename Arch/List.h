#ifndef __LIST__H
#define __LIST__H

#include "Typedef.h"

typedef struct _tagList
{
	struct _tagList* 	m_pNext;
	struct _tagList* 	m_pPre;
    uint8 				m_isUsed;
}List;

void List_Init(List* pList);
List* List_AddTail(List* pHead, List* pNode);
List* List_Remove(List* pNode);
Bool List_isIn(List* pHead, List* pNode);
int List_Count(List* pNode);
void List_RemoveAll(List* pNode);

#endif

