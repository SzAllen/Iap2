
#pragma once
#include "afx.h"

#define CODE_CRC_SIZE 512
#pragma pack(1) 
typedef struct _CrcSector
{
	uint32	m_CodeCrc;
	uint32 	m_FwBinDataLen;	//Firmware code length

	uint8	m_MainVer;
	uint8	m_MinorVer;
	uint8	m_RevisionNum;
	uint32	m_BuildNum;
	
	uint8	m_Reserved2[CODE_CRC_SIZE - 19];
	uint32	m_SectorCrc;			//Firmware sector crc, not include m_SectorCrc
}CrcSector;
#pragma pack()
#define SECTOR_SIZE  512
/*
--------------------------------------------------------------	0x80000		APP_A_START_ADDRESS
	0x1FC00(127k)	A_AREA_CODE
--------------------------------------------------------------	FwCodeLen
	Pad FF
--------------------------------------------------------------	0x9FA00		Pad
	0x1FC00(127k)	CrcSector (512 BYTES)
--------------------------------------------------------------	0x9FC00		APP_B_START_ADDRESS
*/

#define MIN_FILE_LEN	(10*1024)

#define CODE_SIZE 		(120*1024)	//120k
#define BOOT_CODE_SIZE 	(15*1024)	//15k

#define	PAD_CHAR		0xFF

#define CODE_CRC_SIZE 512


class COtaFile :	public CFile
{
	BOOL		m_isOpen;
	BOOL		m_isOta;
public:
	CrcSector	m_CrcSector;
	static const DWORD	CRC_KEY = 0x0000FFFF;
	static BOOL GetOtaFileInfo(LPCTSTR lpFileName, CrcSector& info);
public:
	BOOL isOpen();
	COtaFile(void);
	~COtaFile(void);

	int Append(CFile* pFile);
	int AppendByte(BYTE pad, DWORD nLen);
	int Append(BYTE pad, DWORD nLen);
	int AppendData(const void* pData, DWORD nLen);
	static int Allign(int rawLen, int blockLen = SECTOR_SIZE){return ((rawLen + blockLen - 1)/blockLen + 1) * blockLen;};

	DWORD GetFileCrc();
	int ConvertBleCrcedFile(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, LPCTSTR lpDstFileName, int dstLen = 0);
	int ConvertToBleOta(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, CFile* pDstFile, int dstLen = 0);
	int ConvertCrcedFile(LPCTSTR lpSrcFileName, BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, LPCTSTR lpDstFileName);
	int ConvertCrcedFile(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, LPCTSTR lpDstFileName, int dstLen = 0);
	int ConvertToOta(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, int dstLen = 0);
	int ConvertToOta(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, CFile* pDstFile, int dstLen = 0);
	BOOL IsCrcedFile();
	DWORD CalculateCrc(DWORD start, DWORD end);
	static DWORD CalculateCrc(DWORD& dwCrc, const void* pBuf, int nLen);

public:
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL);
};

