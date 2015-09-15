
#include "Bit.h"

int SearchBit(uint32* pAddr, uint8 nStartIndex, uint8 bitValue)
{
	uint8 i = 0;

	for(i = nStartIndex; i < 32; i++)
	{
		if((((*pAddr)>>i) & 0x01) == (bitValue & 0x01))
		{
			return i;
		}
	}

	return -1;
}

void SetBit(uint32* pAddr, uint8 nBit, uint8 isEnable)
{
	if(nBit > 31) return;
	
	if(isEnable)
	{
		(*pAddr) |= (1 << nBit);
	}
	else
	{
		(*pAddr) &= ~(1 << nBit);
	}
}

void SetBits(uint32* pAddr, uint32 value, uint32 mask)
{
	int i = 0;
	for(i = 0; i < 32; i++)
	{
		if((mask >> i) & 0x1)
		{
			if((value >> i) & 0x1)
			{
				*pAddr |= (1 << i);
			}
			else
			{
				*pAddr &= ~(1 << i);
			}
		}
	}
}

