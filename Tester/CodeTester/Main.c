/******************************************************************************
Main.c
`
描述:
1. 实现项目入口函数功能

创建:
林良军   2013.09.3    建立文件.
******************************************************************************/

#include "Main.h"
#include "WirelessGate.h"
#include "TinyCDMA_Common.h"
#include "TinyCDMA_API.h"
#include "Tlv.h"
#include "Cdma.h"
#include "DevDc.h"
#include "Shell.h"

char bIsI2CReady=FALSE;
char bIsSocketLibReady=FALSE;
char bIsDeviceInfoAvailable=FALSE;
uint32 gDebugLevel = 0;

extern void TinyCDMAInit(UINT8 LocalDeviceType, UINT8 LocalDeviceID, UINT16 FrameID, UINT8 SlotID, UINT8 CPUID, char * CDMA_IP, char * CDMS_IP);
extern void TinyCDMA_TaskInit(void);

//操作系统初始化
void Main_OsInit()
{
}

//硬件初始化
void Main_HwInit()
{
	
}

void Main_StartTimerTask()
{
}

extern void Trace_Task(void * p_arg);
void WirelessGate_StartCdmaTask()
{
	TinyCDMAInit(g_WgInitData.m_WgType
		, g_WgInitData.m_WgNum
		, g_WgInitData.m_CarrierNum
		, g_WgInitData.m_SiteNum
		, g_WgInitData.m_CpuId
		, WG_CDMA_IP
		, WG_CDMS_IP
	);

	TinyCDMA_TaskInit();

	#ifdef PRINT_TO_NEWWORK
	{
	int dwThreadId;
	int m_hWgThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Trace_Task, 0, NULL,(void*)&dwThreadId);
	}
	#endif

}

extern int32 cdm_getTableRecord_1(uint16 iRow, uint16 iTableID, uint8* pData );
extern int32 cdm_getTableRowNum_1(uint16 iTableID);
const CdmaIf g_CdmaIf = {cdm_getTableRecord, cdm_getTableRowNum};
const CdmaIf* g_pCdmaIf = &g_CdmaIf;

void WirelessGate_Task(void * arg1)
{
	Debug_Init();

	PF(DL_MAIN, ("WirelessGateTask is runing...\n"));

	if(!WirelessGate_Init(g_pWg))
	{
		PF(DL_MAIN|DL_WARNING, ("WirelessGate Init FAILED.\n"));
		return;
	}
	
	//获取接入设备信息
	if(!Dc_InitDev(g_pWg))
	{
		PF_FAILED();
		return;
	}

	if(!WirelessGate_Start(g_pWg))
	{
		PF(DL_MAIN|DL_WARNING, ("WirelessGate Init FAILED.\n"));
		return;
	}

	WirelessGate_Run(g_pWg);
}

void Main()
{
	bIsDeviceInfoAvailable = 1;
	bIsI2CReady = 1;
	//g_WgInitData.m_SiteNum = 1;

	Main_OsInit();

	Main_HwInit();

	Debug_Init();

	Printf("\nWirelessGate Firmware(V%d.%d) is runing...\n", g_FirmwareVersion.m_MasterVer, g_FirmwareVersion.m_SubVer);
	Shell_Init();
	//获取DC板信息
	while(0 == g_WgInitData.m_SiteNum)
	{
		//ShowWait(100, 64);
		SLEEP(10);
	}
	Dc_GetIPAddr(&g_WgInitData);

	//获取Board ID
	Printf("********** WirelessGate Info **********\n");
	Printf("\tWgId=%d, WgType=%d, CpuId=%d, CarrierNum=%d, SiteNum=%d\n"
		, g_WgInitData.m_WgNum
		, g_WgInitData.m_WgType
		, g_WgInitData.m_CpuId
		, g_WgInitData.m_CarrierNum
		, g_WgInitData.m_SiteNum);

	//SetSysLwIP(WG_CDMA_IP, NET_MASK, DEFAULT_GW);
	//bIsSocketLibReady = True;
	
 	Printf("\tDcIp\t=[%s]\n \tVogcIp\t=[%s]\n \tBcIp\t=[%s]\n \tMask\t=[%s]\n \tGateWay\t=[%s]\n"
		, WG_CDMA_IP
		, WG_CDMS_IP
		, BC_IP_STR
		, NET_MASK
		, DEFAULT_GW
		);
	Printf("***************************************\n");

	
	bIsI2CReady = True;
	bIsDeviceInfoAvailable = True;
	
	WirelessGate_StartCdmaTask();

	Printf("Cdma is starting.\n\t");
	while(!cdm_IsDataReady())
	{
		//ShowWait(500, 16);
		SLEEP(1);
	}

	Printf("Cdma is ready.\n");
	Dc_Init();
	WirelessGate_Task(0);
}

