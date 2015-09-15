#include "StdAfx.h"
#include "Log.h"


CLog::CLog(void)
{
}


CLog::~CLog(void)
{
}


void CLog::DumpByte(const BYTE* pData, short len, BYTE cols)
{
    int i; 
    int counter = 0;	
    while(counter < len) 
    { 
        for(i=0; i < cols; i++) 
        { 
            Log("%02X ",*pData); 
            pData++; 
            if(++counter >= len) break;	
        } 
        Log("\n"); 
    } 
}
