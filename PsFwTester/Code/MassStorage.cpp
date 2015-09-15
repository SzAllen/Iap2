#include "StdAfx.h"
#include "MassStorage.h"

const int TIMEOUT_VALUE = 20;

#define LOG TRACE

CMassStorage::CMassStorage(void)
{
	m_hFile = INVALID_HANDLE_VALUE;

	m_dwMaxLba		= 0;
	m_SectorSize	= 0;
	m_SenseLen		= 0;
}


CMassStorage::~CMassStorage(void)
{
}



BOOL CMassStorage::Open(WCHAR cName)
{
	m_DriverName.Format(L"\\\\.\\%C:\0",cName);

	CFile::Open(m_DriverName, CFile::modeReadWrite|CFile::shareDenyNone);
	return (m_hFile != INVALID_HANDLE_VALUE);
}

BOOL CMassStorage::IsOpen()
{
	return (m_hFile != INVALID_HANDLE_VALUE);
}

void CMassStorage::Close()
{
	if(IsOpen())
	{
		CloseHandle(m_hFile);
	}

	m_hFile = INVALID_HANDLE_VALUE;
}

BYTE* CMassStorage::GetSenseCode()
{
	if(m_SenseLen)
	{
		return m_SenseBuf;
	}
	else
	{
		return NULL;
	}
}

BOOL CMassStorage::MassStorageIO(PVOID pCBD
	,  BYTE dwCBDLen
	,  DWORD& dwDataTransferLength /* = 0 */
	,  PVOID pBuff /* = NULL */
	,  BYTE cDataIn /* = SCSI_IOCTL_DATA_IN */
	,  BYTE cLun /* =0 */
	)
{
	if(!IsOpen())
	{
		return FALSE;
	}

	if(dwDataTransferLength != 0 && pBuff == NULL)
		return FALSE;

	if(dwCBDLen == 0)
		return FALSE;

	m_SenseLen = 0;

	SCSI_PASS_THROUGH_DIRECT_SENSE    scsiPkt;//bulk-onlyÐ­Òé°ü
	SCSI_PASS_THROUGH_DIRECT* pScsiCmd = (SCSI_PASS_THROUGH_DIRECT*)&scsiPkt;

	ZeroMemory(&scsiPkt,sizeof(SCSI_PASS_THROUGH_DIRECT_SENSE));
	pScsiCmd->Length	= sizeof(SCSI_PASS_THROUGH_DIRECT);//dwDataTransferLength;			
	pScsiCmd->PathId = 0; 
	pScsiCmd->TargetId = 0; 
	pScsiCmd->CdbLength = cLun; 
	pScsiCmd->SenseInfoLength = sizeof(scsiPkt.m_SenseBuf); 
	pScsiCmd->TimeOutValue = TIMEOUT_VALUE; 
	pScsiCmd->SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_SENSE, m_SenseBuf); 
	pScsiCmd->DataIn= cDataIn; 
	pScsiCmd->DataBuffer = pBuff; 
	pScsiCmd->CdbLength = (UCHAR)dwCBDLen;
	memcpy(pScsiCmd->Cdb, pCBD, dwCBDLen);
	pScsiCmd->DataTransferLength = dwDataTransferLength;

	DWORD dwRetLen = 0;
	BOOL bRet = DeviceIoControl(m_hFile, IOCTL_SCSI_PASS_THROUGH_DIRECT,
		&scsiPkt, sizeof(SCSI_PASS_THROUGH_DIRECT_SENSE),
		&scsiPkt, sizeof(SCSI_PASS_THROUGH_DIRECT_SENSE), &dwRetLen, NULL);

	if(!bRet || pScsiCmd->ScsiStatus != 0)
	{
		DWORD no = GetLastError();
		return FALSE;
	}

	dwDataTransferLength = pScsiCmd->DataTransferLength;

	if(!bRet && pScsiCmd->SenseInfoLength)
	{
		m_SenseLen = pScsiCmd->SenseInfoLength;
		memcpy(m_SenseBuf, scsiPkt.m_SenseBuf, m_SenseLen);
	}
	ZeroMemory(&scsiPkt,sizeof(SCSI_PASS_THROUGH_DIRECT_SENSE));


	return TRUE;
}

BOOL CMassStorage::DataIn(const PVOID pCBD, BYTE dwCBDLen, const PVOID pData, DWORD& dwDataLen, BYTE cLun)
{
	return MassStorageIO(pCBD, dwCBDLen, dwDataLen, pData, SCSI_IOCTL_DATA_IN, cLun);
}

BOOL CMassStorage::DataOut(const PVOID pCBD, BYTE dwCBDLen, PVOID pBuff, DWORD dwBuffLen, BYTE cLun)
{
	return MassStorageIO(pCBD, dwCBDLen, dwBuffLen, pBuff, SCSI_IOCTL_DATA_OUT, cLun);
}

BOOL CMassStorage::DataNone(const PVOID pCBD, BYTE dwCBDLen, BYTE cLun)
{
	DWORD dwBuffLen = 0;

	return MassStorageIO(pCBD, dwCBDLen, dwBuffLen, NULL, SCSI_IOCTL_DATA_IN, cLun);
}

BOOL CMassStorage::ExeCmd(BYTE cmd, BYTE cLun)
{
	DWORD dwBuffLen = 0;

	return MassStorageIO(&cmd, 1, dwBuffLen, NULL, SCSI_IOCTL_DATA_IN, cLun);
}

//SCSI cmd
BOOL CMassStorage::Inquiry(SCSIINQUIRY* pstrScsiInquiryResult, DWORD& iBufLen,  BYTE cLun)
{
	BYTE pbCBD[12] = {0};
    pbCBD[0] = MSC_INQUIRY;
	pbCBD[4] = 0x24;

	BOOL bRet = DataIn((PVOID)pbCBD, 6, (PVOID)pstrScsiInquiryResult, iBufLen, cLun);
	if(bRet)
	{
		//memcpy(&m_strScsiinquiry, pstrScsiInquiryResult, sizeof(SCSIINQUIRY));
	}
	else
	{
		LOG(("Send MSC_READ_CAPACITY Failed.\n"));
	}

	return bRet;
}


BOOL CMassStorage::ReadCapacity(DWORD& maxLba,  UINT16& sectorSize, BYTE cLun)
{
	BYTE inBuff[8] = {0, 0};
	DWORD inLen = sizeof(inBuff);

	BYTE pbCmd[12] = {'\0'};
	pbCmd[0] = MSC_READ_CAPACITY;

	BOOL bRet = DataIn(pbCmd, 12, inBuff, inLen, cLun);
	
	if(bRet)
	{
		if(inLen>=8)
		{
			maxLba		= AS_UINT32(inBuff[0],inBuff[1],inBuff[2],inBuff[3]);
			sectorSize	= AS_UINT32(inBuff[4],inBuff[5],inBuff[6],inBuff[7]);

			m_dwMaxLba		= maxLba;
			m_SectorSize	= sectorSize;
		}
		else
		{
			LOG(("Send MSC_READ_CAPACITY Failed.\n"));
		}
	}

	return bRet;
}

BOOL CMassStorage::Read10(DWORD lba,  UINT16 sectors,  PVOID pBuffer,  DWORD& dwBuffLen, BYTE cLun)
{
	BYTE pbCmd[12] = {SCSIOP_READ, 0};

	pbCmd[2] = (BYTE)(lba >> 24);
	pbCmd[3] = (BYTE)(lba >> 16);
	pbCmd[4] = (BYTE)(lba >> 8);
	pbCmd[5] = (BYTE)lba;

	pbCmd[7] = (BYTE)(sectors >> 8);
	pbCmd[8] = (BYTE)sectors;

	return  DataIn(pbCmd, 12, pBuffer, dwBuffLen, cLun);
}

BOOL CMassStorage::Write10(DWORD lba,  UINT16 sectors,  PVOID pBuffer,  DWORD dwBuffLen, BYTE cLun)
{
	BYTE pbCmd[12] = {SCSIOP_WRITE, 0};

	pbCmd[2] = (BYTE)(lba >> 24);
	pbCmd[3] = (BYTE)(lba >> 16);
	pbCmd[4] = (BYTE)(lba >> 8);
	pbCmd[5] = (BYTE)lba;

	pbCmd[7] = (BYTE)(sectors >> 8);
	pbCmd[8] = (BYTE)sectors;

	return DataOut(pbCmd, 12, pBuffer, dwBuffLen, cLun);
}

BOOL CMassStorage::TestUnitReady(BYTE cLun)
{
	BYTE pbCmd[12] = {SCSIOP_TEST_UNIT_READY, 0};
	m_IsUnitReady = DataNone(pbCmd, 1);

	return m_IsUnitReady;
}

