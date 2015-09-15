#pragma once

#include <WINIOCTL.H>
#include <Ntddscsi.h>

typedef struct _SCSI_PASS_THROUGH_DIRECT_SENSE 
{
	SCSI_PASS_THROUGH_DIRECT m_Base;
	DWORD m_Reserved;	// Align buffer to double word boundary
	BYTE m_SenseBuf[16];
}SCSI_PASS_THROUGH_DIRECT_SENSE, *P_SCSI_PASS_THROUGH_DIRECT_SENSE;

typedef struct _SCSIINQUIRY
{
    unsigned char  m_cDeviceType    :5;     //外围设备类型
    unsigned char  m_cReserved0     :3;
	
    unsigned char  m_cReserved1     :7;
    unsigned char  m_cRmb           :1;     //盘符可见标志
	
    unsigned char  m_cAnsiVersion   :3;
    unsigned char  m_cEcmaVersion   :3;
    unsigned char  m_cIsoVesion     :2;
	
    unsigned char  m_cRepDataFormat :4;
    unsigned char  m_cReserved2     :4;
	
    unsigned char  m_cAdditionalLen;
    unsigned char  m_cReserved3[3];
    unsigned char  m_pbVendorInfo[8];        //V_NAME
    unsigned char  m_pbProductInfo[16];        //P_NAME  
    unsigned char  m_pbProductRevisionlevel[4]; //固件版本号
	
    unsigned char  m_pbVendorSpec[20];
    unsigned char  m_cReserved4;
    unsigned char  m_pbVendorSpecPara[64];
}SCSIINQUIRY, *PSCSIINQUIRY;

class CMassStorage: public CFile
{
protected:
	CString		m_DriverName;

	DWORD		m_dwMaxLba;
	UINT16		m_SectorSize;

	BYTE		m_SenseLen;
	BYTE		m_SenseBuf[16];

	BOOL		m_IsUnitReady;
public:

public:
	CMassStorage(void);
	~CMassStorage(void);

public:
	BOOL Open(WCHAR cName);
	void Close();
	BOOL IsOpen();

	BOOL MassStorageIO(PVOID pCBD
		, BYTE dwCBDLen
		, DWORD& dwDataTransferLength
		, PVOID pBuff  = NULL
		, BYTE cDataIn = SCSI_IOCTL_DATA_IN
		, BYTE cLun = 0			
		);

	BYTE* GetSenseCode();

	BOOL DataIn  (const PVOID pCBD, BYTE dwCBDLen, const PVOID pData, DWORD& dwDataLen, BYTE cLun = 0);
	BOOL DataOut (const PVOID pCBD, BYTE dwCBDLen, PVOID pBuff, DWORD dwBuffLen, BYTE cLun = 0);
	BOOL DataNone(const PVOID pCBD, BYTE dwCBDLen, BYTE cLun = 0);
	BOOL ExeCmd(BYTE cmd, BYTE cLun = 0);

	BOOL Inquiry(SCSIINQUIRY* pstrScsiInquiryResult, DWORD& iBufLen,  BYTE cLun = 0);
	BOOL ReadCapacity(DWORD& maxLba,  UINT16& sectorSize, BYTE cLun = 0);
	BOOL Read10 (DWORD lba,  UINT16 sectors,  PVOID pBuffer,  DWORD& dwBuffLen, BYTE cLun = 0);
	BOOL Write10(DWORD lba,  UINT16 sectors,  PVOID pBuffer,  DWORD dwBuffLen, BYTE cLun = 0);
	BOOL TestUnitReady(BYTE cLun = 0);

};

