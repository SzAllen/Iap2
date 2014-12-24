
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

#ifdef WIN32
CRITICAL_SECTION g_CriticalSection;
int Osa_Init()
{
	InitializeCriticalSection(&g_CriticalSection);

	return 0;
}
#else
int Osa_Init()
{
	return 0;
}
#endif

