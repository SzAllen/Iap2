#ifndef __TYPEDEF__H_
#define __TYPEDEF__H_

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _WINDOWS
#else
#include "Compiler.h"
#endif

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef volatile unsigned int vuint32;
typedef int int32;
typedef uint16 wchar;

#ifdef _WINDOWS
#ifdef Bool
#undef Bool
#endif
#define Bool uint8
#endif

#define True 	1
#define False 	0
#define Null	0

//#define inline __inline 
#define _FUNC_ __FUNCTION__ 
#define _FILE_ __FILE__ 
#define _LINE_ __LINE__ 


#ifdef _WINDOWS
#else

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	0
#endif

#endif

/* BITS */
#define BIT_0		0x00000001UL
#define BIT_1		0x00000002UL
#define BIT_2		0x00000004UL
#define BIT_3		0x00000008UL
#define BIT_4		0x00000010UL
#define BIT_5		0x00000020UL
#define BIT_6		0x00000040UL
#define BIT_7		0x00000080UL
#define BIT_8		0x00000100UL
#define BIT_9		0x00000200UL
#define BIT_10		0x00000400UL
#define BIT_11		0x00000800UL
#define BIT_12		0x00001000UL
#define BIT_13		0x00002000UL
#define BIT_14		0x00004000UL
#define BIT_15		0x00008000UL
#define BIT_16		0x00010000UL
#define BIT_17		0x00020000UL
#define BIT_18		0x00040000UL
#define BIT_19		0x00080000UL
#define BIT_20		0x00100000UL
#define BIT_21		0x00200000UL
#define BIT_22		0x00400000UL
#define BIT_23		0x00800000UL
#define BIT_24		0x01000000UL
#define BIT_25		0x02000000UL
#define BIT_26		0x04000000UL
#define BIT_27		0x08000000UL
#define BIT_28		0x10000000UL
#define BIT_29		0x20000000UL
#define BIT_30		0x40000000UL
#define BIT_31		0x80000000UL

#define SECTOR_FACTOR			9

#define BIT(_x, _i) ((((uint32)(_x)) >> (_i)) & 0x00000001)
#define AS_UINT8(x) (*((uint8*)&(x)))
#define AS_UINT16(_msByte, _lsByte) (uint16)((((_msByte) << 8)) | (_lsByte))
#define AS_UINT32(_Byte3, _Byte2,_Byte1, _Byte0) (uint32)(((_Byte3) << 24) | ((_Byte2) << 16) | ((_Byte1) << 8) | (_Byte0))

//´óÐ¡¶Ë×ª»»
#define SWAP16(value) (uint16)(((value) << 8) | (((value) >> 8) & 0xFF))
#define SWAP32(value) (uint32)((((uint8*)&(value))[0] << 24) | (((uint8*)&(value))[1] << 16) | (((uint8*)&(value))[2] << 8) | ((uint8*)&(value))[3])

#define MEM_BYTE(asUint32, byteIndex) (((uint8*)&(asUint32))[byteIndex])

#define U16_MSB(asUint16) (uint8)((asUint16)>>8)
#define U16_LSB(asUint16) (uint8)(asUint16)

#ifdef BIG_ENDIAN
#define N2H16(value) (value)
#define N2H32(value) (value)

#define H2N16(value) (value)
#define H2N32(value) (value)
#else
#define N2H16(value) SWAP16(value)
#define N2H32(value) SWAP32(value)

#define H2N16(value) SWAP16(value)
#define H2N32(value) SWAP32(value)
#endif

#ifdef __cplusplus
}
#endif

#endif

