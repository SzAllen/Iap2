
#include "ArchDef.h"
#include "Debug.h"


#if XDEBUG

unsigned int g_dwDebugLevel = 0;

void Debug_Init()
{
	g_dwDebugLevel = DL_MAIN;
	g_dwDebugLevel |= DL_WARNING;
	g_dwDebugLevel |= DL_ASSERT;
}

void Debug_EnbaleLevel(uint32 level, Bool isEnable)
{
	if(isEnable)
	{
		g_dwDebugLevel |= level	;
	}
	else
	{
		g_dwDebugLevel &= level	;
	}
}

void Debug_DumpByteEx(uint32 level, const uint8* pData, uint16 len, uint8 cols, const char* str, int line) 
{
    int i; 
    int counter = 0;	

	if(!(level & g_dwDebugLevel)) return;
	
	if(str && line)
	{
		Printf("%s,line[%d],Len[%d]:",str,line,len); 
	}
	else if(str)
	{
		Printf("%s,Len[%d]:",str,len); 
	}
	else if(line)
	{
		Printf("line[%d],Len[%d]:",line,len); 
	}
	
    while(counter < len) 
    { 
        for(i=0; i < cols; i++) 
        { 
            Printf("%02x ",*pData); 
            pData++; 
            if(++counter >= len) break;	
        } 
    } 
    Printf("\n"); 
}

void ShowWait(int ticksPerDot, int dotCountPerRow)
{
	static int i = 0;
	static int count = 0;

	if( (i++) % ticksPerDot == 0)
	{
		if(count++ >= dotCountPerRow)
		{
			count = 0;
			i = 0;
			Printf("\n\t");
		}
		else
		{
			Printf(".");
		}
	}
}

void Debug_DumpByte(const uint8* pData, uint16 len, uint8 cols) 
{
    int i; 
    int counter = 0;	
	//Printf("Dump Data, addr=[0x%04x], Len=%d\n", pData, len); 
    while(counter < len) 
    { 
        //Printf("[%04x]:",counter); 
        for(i=0; i < cols; i++) 
        { 
            Printf("%02x ",*pData); 
            pData++; 
            if(++counter >= len) break;	
        } 
        Printf("\n"); 
    } 
}

void Debug_DumpDword(const uint32* pData, uint16 len, uint8 cols) 
{
    int i; 
    int counter = 0;	
    while(counter < len) 
    { 
        Printf("[%04x]:",counter); 
        for(i=0; i < cols; i++) 
        { 
            Printf("%08x ",*pData); 
            pData++; 
			
            if(++counter >= len) break;	
        } 
        Printf("\n"); 
    } 
}

#endif

