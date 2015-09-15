#include "stdafx.h"
#include "OtaFile.h"

static DWORD m_dwCrcTable[256];

void InitCrcTable()
{
	DWORD c;
	for(DWORD i = 0; i < 256; i++)
	{
		c = i;
		for (DWORD j = 0; j < 8; j++) 
		{  
            if (c & 1)
			{
                c = 0xedb88320 ^ (c >> 1);
			}
            else
			{
                c = c >> 1;
			}
        }  
        m_dwCrcTable[i] = c;  
	}
}

COtaFile::COtaFile(void)
{
	m_isOpen	= FALSE;
	m_isOta		= FALSE;
	memset(&m_CrcSector, 0, sizeof(m_CrcSector));
	InitCrcTable();
}


COtaFile::~COtaFile(void)
{
}

BOOL COtaFile::isOpen()
{
	return (m_hFile != INVALID_HANDLE_VALUE);
}

BOOL COtaFile::IsCrcedFile()
{
	ASSERT(isOpen());

	return m_isOta;
}

int COtaFile::AppendData(const void* pData, DWORD nLen)
{
	ASSERT(isOpen());
	DWORD dwFileLen = (DWORD)GetLength();

	SeekToEnd();

	int rwLen = 0;
	DWORD copiedLen = nLen;

	for(copiedLen = nLen; copiedLen > 0; copiedLen -= rwLen)
	{
		rwLen = copiedLen > SECTOR_SIZE ? SECTOR_SIZE : copiedLen;

		Write(pData, rwLen);
	}

	return nLen;
}

int COtaFile::AppendByte(BYTE pad, DWORD nLen)
{
	SeekToEnd();

	int rwLen = 0;
	DWORD copiedLen = 0;
	BYTE pbBuffer[SECTOR_SIZE];

	memset(pbBuffer, pad, nLen);

	for(copiedLen = nLen; copiedLen > 0; copiedLen -= rwLen)
	{
		rwLen = copiedLen > SECTOR_SIZE ? SECTOR_SIZE : copiedLen;

		Write(pbBuffer, rwLen);
	}

	return (int)GetLength();
}

int COtaFile::Append(BYTE pad, DWORD nLen)
{
	ASSERT(isOpen());
	DWORD dwFileLen = (DWORD)GetLength();

	ASSERT(nLen >= dwFileLen);

	SeekToEnd();

	int rwLen = 0;
	DWORD copiedLen = dwFileLen;
	BYTE pbBuffer[SECTOR_SIZE];

	for(copiedLen = nLen - dwFileLen; copiedLen > 0; copiedLen -= rwLen)
	{
		rwLen = copiedLen > SECTOR_SIZE ? SECTOR_SIZE : copiedLen;

		memset(pbBuffer, pad, rwLen);
		Write(pbBuffer, rwLen);
	}

	return nLen;
}

int COtaFile::Append(CFile* pFile)
{
	ASSERT(isOpen());

	SeekToEnd();

	int rwLen = 0;
	DWORD nSrcLen = (DWORD)GetLength();
	DWORD dwFileLen = (DWORD)pFile->GetLength();
	DWORD copiedLen = dwFileLen;
	BYTE pbBuffer[SECTOR_SIZE];

	pFile->SeekToBegin();
	for(copiedLen = dwFileLen; copiedLen > 0; copiedLen -= rwLen)
	{
		rwLen = copiedLen > SECTOR_SIZE ? SECTOR_SIZE : copiedLen;

		if(pFile->Read(pbBuffer, rwLen))
		{
			Write(pbBuffer, rwLen);
		}
		else
		{
			TRACE(L"Read file data failed.\n");
			return 0;
		}
	}

	return dwFileLen + nSrcLen;
}
 
int COtaFile::ConvertToOta(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, CFile* pDstFile, int dstLen)
{
	DWORD dwFileLen = 0;
	BYTE pbBuffer[SECTOR_SIZE];
	DWORD dwCrc = CRC_KEY;

	ASSERT(sizeof(CrcSector) == SECTOR_SIZE);

	if(dstLen == 0) 
		dstLen = Allign((int)GetLength(), SECTOR_SIZE);
	
	SeekToBegin();

	for(DWORD i = 0; i < ( GetLength() / SECTOR_SIZE); i++)
	{
		if(Read(pbBuffer, SECTOR_SIZE) != SECTOR_SIZE)
		{
			TRACE(L"Read file failed.\n");
		}
		CalculateCrc(dwCrc, pbBuffer, SECTOR_SIZE);

		pDstFile->Write(pbBuffer, SECTOR_SIZE);
		dwFileLen += SECTOR_SIZE;
	}

	//Write the residue data
	int residueLen = GetLength() % SECTOR_SIZE;
	if(residueLen)
	{
		memset(&pbBuffer, PAD_CHAR, SECTOR_SIZE);
		Read(pbBuffer, residueLen);
		CalculateCrc(dwCrc, pbBuffer, residueLen);

		pDstFile->Write(pbBuffer, SECTOR_SIZE);
		dwFileLen += SECTOR_SIZE;
	}

	DWORD padSectorCount = (dstLen - dwFileLen)/SECTOR_SIZE;
	for(DWORD i = 0; i < padSectorCount; i++)
	{
		memset(&pbBuffer, PAD_CHAR, SECTOR_SIZE);
		if(i == padSectorCount-1)
		{
			memset(&pbBuffer, PAD_CHAR, SECTOR_SIZE);
			CrcSector* pSector = (CrcSector*)pbBuffer;
			pSector->m_FwBinDataLen	= (DWORD)GetLength();

			pSector->m_CodeCrc		= dwCrc;
			pSector->m_MainVer		= mainVer;
			pSector->m_MinorVer		= minorVer;
			pSector->m_RevisionNum	= revisionNum;
			pSector->m_BuildNum		= buildNum;

			dwCrc = CRC_KEY;
			CalculateCrc(dwCrc, (BYTE*)pSector, SECTOR_SIZE-4);
			pSector->m_SectorCrc = dwCrc;
		}

		pDstFile->Write(pbBuffer, SECTOR_SIZE);
		dwFileLen += SECTOR_SIZE;
	}

	return dwFileLen;
}

int COtaFile::ConvertToOta(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, int dstLen)
{
	DWORD dwFileLen = 0;
	BYTE pbBuffer[SECTOR_SIZE];
	DWORD dwCrc = CRC_KEY;

	ASSERT(sizeof(CrcSector) == SECTOR_SIZE);

	if(dstLen == 0) 
		dstLen = Allign(dwFileLen, SECTOR_SIZE);
	
	//Write the residue data
	SeekToEnd();
	int residueLen = GetLength() % SECTOR_SIZE;
	if(residueLen)
	{
		memset(&pbBuffer, PAD_CHAR, residueLen);
		CalculateCrc(dwCrc, pbBuffer, residueLen);

		Write(pbBuffer, residueLen);
		dwFileLen += residueLen;
	}

	DWORD padSectorCount = (dstLen - dwFileLen)/SECTOR_SIZE;
	for(DWORD i = 0; i < padSectorCount; i++)
	{
		memset(&pbBuffer, PAD_CHAR, SECTOR_SIZE);
		if(i == padSectorCount-1)
		{
			CrcSector* pSector = (CrcSector*)pbBuffer;
			pSector->m_FwBinDataLen	= (DWORD)GetLength();

			pSector->m_CodeCrc		= dwCrc;
			pSector->m_MainVer		= mainVer;
			pSector->m_MinorVer		= minorVer;
			pSector->m_RevisionNum	= revisionNum;
			pSector->m_BuildNum		= buildNum;

			dwCrc = CRC_KEY;
			CalculateCrc(dwCrc, (BYTE*)pSector, SECTOR_SIZE-4);
			pSector->m_SectorCrc = dwCrc;
		}

		Write(pbBuffer, SECTOR_SIZE);
		dwFileLen += SECTOR_SIZE;
	}

	return dwFileLen;
}

int COtaFile::ConvertCrcedFile(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, LPCTSTR lpDstFileName, int dstLen)
{
	int nLen = 0;
	if(!isOpen())
	{
		return 0;
	}

	CFile fDst;

	if(!fDst.Open(lpDstFileName, CFile::modeCreate|CFile::modeReadWrite))
	{
		CString str;str.Format(L"Open file[%s] failed!", lpDstFileName);

		MessageBox(NULL, str, L"Error", MB_OK|MB_ICONSTOP);
		return 0;
	}

	nLen = ConvertToOta(mainVer, minorVer, revisionNum, buildNum, &fDst, dstLen);
	fDst.Close();

	return nLen;
}
int COtaFile::ConvertToBleOta(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, CFile* pDstFile, int dstLen)
{
	const DWORD dwOriginalFileLen = (DWORD)GetLength();
	DWORD dwSrcFileLen = dwOriginalFileLen;
	DWORD dwFileLen = 0;
	BYTE pbBuffer[SECTOR_SIZE];
	DWORD dwCrc = CRC_KEY;

	ASSERT(sizeof(CrcSector) == SECTOR_SIZE);

	if(dstLen == 0) 
		dstLen = Allign((int)dwOriginalFileLen + SECTOR_SIZE - 18, SECTOR_SIZE);
	
	//SeekToBegin();
	//Discard the 2 byte in head
	Seek(2, 0);
	dwSrcFileLen -= 2;

	const static int HEAD_LEN = 16;
	//Read 16 bytes,and pad 1 secotr
	memset(&pbBuffer, PAD_CHAR, SECTOR_SIZE);
	if(Read(pbBuffer, HEAD_LEN) != HEAD_LEN)
	{
		TRACE(L"Read file failed.\n");
	}
	memcpy(&pbBuffer[HEAD_LEN], &dwOriginalFileLen, sizeof(dwSrcFileLen));
	dwSrcFileLen -= HEAD_LEN;

	CalculateCrc(dwCrc, &pbBuffer, SECTOR_SIZE);
	pDstFile->Write(&pbBuffer, SECTOR_SIZE);
	dwFileLen += SECTOR_SIZE;

	for(DWORD i = 0; i < ( dwSrcFileLen / SECTOR_SIZE); i++)
	{
		if(Read(pbBuffer, SECTOR_SIZE) != SECTOR_SIZE)
		{
			TRACE(L"Read file failed.\n");
		}
		CalculateCrc(dwCrc, pbBuffer, SECTOR_SIZE);

		pDstFile->Write(pbBuffer, SECTOR_SIZE);
		dwFileLen += SECTOR_SIZE;
	}

	//Write the residue data
	int residueLen = (dwSrcFileLen) % SECTOR_SIZE;
	if(residueLen)
	{
		memset(&pbBuffer, PAD_CHAR, SECTOR_SIZE);
		int len = Read(pbBuffer, residueLen);
		CalculateCrc(dwCrc, pbBuffer, residueLen);

		pDstFile->Write(pbBuffer, SECTOR_SIZE);
		dwFileLen += SECTOR_SIZE;
	}

	DWORD padSectorCount = (dstLen - dwFileLen)/SECTOR_SIZE;
	for(DWORD i = 0; i < padSectorCount; i++)
	{
		memset(&pbBuffer, PAD_CHAR, SECTOR_SIZE);
		if(i == padSectorCount-1)
		{
			memset(&pbBuffer, 0, SECTOR_SIZE);
			CrcSector* pSector = (CrcSector*)pbBuffer;
			pSector->m_FwBinDataLen	= dwOriginalFileLen;

			pSector->m_CodeCrc		= dwCrc;
			pSector->m_MainVer		= mainVer;
			pSector->m_MinorVer		= minorVer;
			pSector->m_RevisionNum	= revisionNum;
			pSector->m_BuildNum		= buildNum;

			dwCrc = CRC_KEY;
			CalculateCrc(dwCrc, (BYTE*)pSector, SECTOR_SIZE-4);
			pSector->m_SectorCrc = dwCrc;
		}

		pDstFile->Write(pbBuffer, SECTOR_SIZE);
		dwFileLen += SECTOR_SIZE;
	}

	return dwFileLen;
}

int COtaFile::ConvertBleCrcedFile(BYTE mainVer, BYTE minorVer, BYTE revisionNum, DWORD buildNum, LPCTSTR lpDstFileName, int dstLen)
{
	int nLen = 0;
	if(!isOpen())
	{
		return 0;
	}

	CFile fDst;

	if(!fDst.Open(lpDstFileName, CFile::modeCreate|CFile::modeReadWrite))
	{
		CString str;str.Format(L"Open file[%s] failed!", lpDstFileName);

		MessageBox(NULL, str, L"Error", MB_OK|MB_ICONSTOP);
		return 0;
	}

	nLen = ConvertToBleOta(mainVer, minorVer, revisionNum, buildNum, &fDst, dstLen);
	fDst.Close();

	return nLen;
}
#if 0
BOOL COtaFile::IsCrcedFile(CrcSector* pCrcSector)
{
	DWORD dwCrc = CRC_KEY;
	CrcSector sector = {0};
	BYTE byte[SECTOR_SIZE] = {0};
	CString str;

	DWORD dwFileLen = (DWORD)GetLength();

	Seek(dwFileLen - SECTOR_SIZE, 0);
	Read(&sector, SECTOR_SIZE);

	SeekToBegin();
	if(sector.m_FwCodeSectorCount * SECTOR_SIZE > CODE_SIZE)
	{
		return FALSE;
	}

	for(DWORD i = 0; i < sector.m_FwCodeSectorCount; i ++)
	{
		Read(byte, SECTOR_SIZE);
		CalculateCrc(dwCrc, byte, SECTOR_SIZE);
	}

	if(pCrcSector) memcpy(pCrcSector, &sector, sizeof(sector));

	return dwCrc == sector.m_CodeCrc;
}
#endif

DWORD COtaFile::CalculateCrc(DWORD& dwCrc, const void* pData, int nLen)
{
	BYTE* pBuf = (BYTE*)pData;
    int i;
    SHORT crc = (SHORT)dwCrc;

    for (i = 0; i < nLen; i++)
    {
        crc  = (unsigned char)(crc >> 8) | (crc << 8);
        crc ^= pBuf[i];
        crc ^= (unsigned char)(crc & 0xff) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xff) << 4) << 1;
    }

	dwCrc = crc & 0x0000FFFF;
    return crc;
}

DWORD COtaFile::CalculateCrc(DWORD startPos, DWORD endPos)
{
	DWORD dwFileLen = (DWORD)GetLength();

	ASSERT(startPos < endPos && dwFileLen <= endPos);

	Seek(startPos, CFile::begin);

	BYTE buffer[SECTOR_SIZE];
	DWORD dwCrc = CRC_KEY;

	int sectors = (endPos - startPos) / SECTOR_SIZE;
	for(int i = 0; i < sectors; i++)
	{
		Read(buffer, SECTOR_SIZE);
		CalculateCrc(dwCrc, buffer, SECTOR_SIZE);
	}

	int residueLen = (endPos - startPos) % SECTOR_SIZE;
	if(residueLen)
	{
		Read(buffer, residueLen);
		CalculateCrc(dwCrc, buffer, residueLen);
	}

	return dwCrc;
}

BOOL COtaFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	if(!CFile::Open(lpszFileName, nOpenFlags, pError)) return FALSE;

	DWORD fileLen = (DWORD)GetLength();
	if(fileLen < MIN_FILE_LEN)
	{
		m_isOta = FALSE;
		return TRUE;
	}

	Seek(fileLen - SECTOR_SIZE, CFile::begin);
	Read(&m_CrcSector, sizeof(m_CrcSector));

	DWORD crc = CRC_KEY;
	CalculateCrc(crc, &m_CrcSector, sizeof(m_CrcSector) - 4);

	m_isOta = (m_CrcSector.m_SectorCrc == crc);

	return TRUE;
}

BOOL COtaFile::GetOtaFileInfo(LPCTSTR lpszFileName, CrcSector& info)
{
	COtaFile file;
	if(!file.Open(lpszFileName, CFile::modeRead) || !file.m_isOta)
		return FALSE;

	memcpy(&info, &file.m_CrcSector, sizeof(CrcSector));

	return TRUE;
}
