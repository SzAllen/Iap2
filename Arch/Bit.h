#ifndef __BIT__H__
#define __BIT__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
  
int SearchBit(uint32* pAddr, uint8 nStartIndex, uint8 bitValue/*0 or 1*/);
void SetBit(uint32* pAddr, uint8 nBit, uint8 bitValue/*0 or 1*/);
void SetBits(uint32* pAddr, uint32 value, uint32 mask);

#ifdef __cplusplus
}
#endif

#endif


