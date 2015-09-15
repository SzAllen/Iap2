
#include "ArchDef.h"

void Uint32Set(uint32* pAddr, uint32 value, uint32 mask)
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

int Osa_Init(void)
{
	//Do nothing
	return 0;
}

