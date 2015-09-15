#ifdef CONFIG_CMDLINE

#include "CmdLine.h"
#include "Shell.h"
#include "Ble.h"
#include "Str.h"
#include "Mcu.h"
#include "gpbr.h"
#include "Boot.h"
#include "PwdMgr.h"
#include "Nvd.h"
#include "boot_config.h"
#include "boot.h"
#include "Iap.h"

#include "Usart.h"
extern UtpMgr g_MtpUtpMgr;

int GetBit(uint32 value)
{
	int i = 0; 
	for(i = 0; i < 32; i++)
	{
		if(value == (1<<i)) 
			return i;
	}

	return 0;
}

static void Shell_SetDebugBit(uint32 nIndex, Bool isEnable)
{
	#define OUT(value) Printf("g_dwDebugLevel.[BIT:%02d].%s\t = %d.\n", GetBit(value), #value, (g_dwDebugLevel >> GetBit(value)) & 0x01)

	if(isEnable)
	{
		g_dwDebugLevel |= (1 << nIndex);
	}
	else
	{
		g_dwDebugLevel &= ~(1 << nIndex);
	}

	OUT(DL_MAIN);
	OUT(DL_ERROR	);
	OUT(DL_WARNING);
	OUT(DL_ASSERT	);
	OUT(DL_LISTEX	);
	OUT(DL_TIMER	);
	OUT(DL_PIPE	);
	OUT(DL_USB_DRV);
	OUT(DL_CRB	);
	OUT(DL_EP0	);
	OUT(DL_BULK_EP);
	OUT(DL_USBDEV	);
	OUT(DL_USBHOST);
	OUT(DL_BOT);
	OUT(DL_SCSI);
	OUT(DL_MEDIA);
	OUT(DL_IAP);
	OUT(DL_CUSTOM);
	OUT(DL_BLE);
	OUT(DL_UTP);
	OUT(DL_USART);
	OUT(DL_CUSTOM_DATA);
	OUT(DL_QN_OTA);
	
	OUT(DL_KEY);
	OUT(DL_BOOT);
	OUT(DL_SHOW_CALLED);
	OUT(DL_SHOW_TICKS	);

	g_OemInfo.m_dwDebugLevel = g_dwDebugLevel;
	PF(DL_MAIN, ("g_dwDebugLevel = 0x%08x\n", g_dwDebugLevel));
	Boot_WriteOemInfo(&g_OemInfo);
}

void Shell_BlePostCmd(uint8 cmd, int len, uint8 data1, uint8 data2)
{
	uint8 buff[2];
	buff[0] = data1;
	buff[1] = data2;
	Ble_PostReq(cmd, buff, len);
}

void Shell_SetPmu(uint8 pmuState)
{
	Ble_RcvState(pmuState);
}

void Shell_Reset()
{
	Ble_SendCmd(REQ_VOUT_DISCNT, Null, 0, 5);
	Mcu_reset();
}

void Shell_SetMcuState(uint8 flag)
{
	Mcu_SetState(MCU_IDLE);
}

void Shell_GetOemInfo()
{
	memset(&g_OemInfo, 0, sizeof(OemInfo));
	Boot_ReadOemInfo(&g_OemInfo);
	PF(DL_MAIN, ("\tTestMode = %s.\n", g_OemInfo.m_isTestMode ? "TURE" : "FALSE"));
	PF(DL_MAIN, ("\tisDefaultPrintf = %s.\n", g_OemInfo.m_isDefaultPrintf ? "TURE" : "FALSE"));
}

void Shell_SetPrintfFlag(Bool isDefaultPrintf)
{
	if(g_OemInfo.m_isDefaultPrintf != isDefaultPrintf)
	{
		g_OemInfo.m_isDefaultPrintf = isDefaultPrintf;
		g_OemInfo.m_dwDebugLevel = 1;
		Boot_WriteOemInfo(&g_OemInfo);
	}

	Shell_GetOemInfo();
}

void Shell_GetTempPwdEmail(uint8_t *seed)
{
	uint8 buff[SECTOR_SIZE] = {0};
	
	PwdMgr_GetTempPwdEmail(buff, seed);

	PF(DL_MAIN, ("Email = %s; Pwd=%s; isTempPwd=%d.\n", buff, &buff[256], g_pNvd->m_isTempPsw));		
}

void Shell_PrintNvd()
{	
	Nvd_Dump();
}

void Shell_SetSyncLedEnable(Bool isEnable, Bool isModifiedSdCardByPc)
{
	Nvd_WriteRunInfo(isEnable, isModifiedSdCardByPc);
	
	Nvd_Dump();
}

void Shell_BleEnable(Bool isEnable)
{
	g_pBle->m_isActiveNew = isEnable;
	
	//Nvd_Write(g_pNvd);
	
	PF(DL_MAIN, ("isBleEnable = %d\n", g_pBle->m_isActiveNew));	
}
void Shell_NvdErase()
{
	Nvd_Reset(g_pNvd);
}

void Shell_VerifyPsw(uint8* pPsw)
{
	if(g_pNvd->m_isPswEnalbe)
	{
		g_pNvd->m_isVerify = PwdMgr_VerifyPwd(pPsw);
		if(g_pNvd->m_isVerify && PWR_PC == g_pBle->m_Pmu.m_pwr_type)
		{
			Arch_PostMsg(g_pMcu, MSG_PSW_VERIFY_OK, 0, True);
		}
		else
		{
			Arch_PostMsg(g_pMcu, MSG_PSW_VERIFY_OK, 0, False);
		}
	}
}

void Shell_SetPwdEnalbe(Bool isEnalbe)
{
	Nvd_WriteSecuInfoEnable(isEnalbe);
	Nvd_Dump();
}

void Shell_WriteSecuInfo(const char* pPsw, const char* pEmail, Bool isTempPsw)
{
	Nvd_WriteSecuInfo(pPsw, pEmail, isTempPsw);
	Nvd_Dump();
}

void Shell_SetPhoneID(const char* pPhoneId)
{
	Nvd_WriteConnectedDevInfo((uint8*)pPhoneId, strlen(pPhoneId));
	Nvd_Dump();
}

void Shell_SetBootFlag(Bool isAArea)
{
	Boot_WriteBootFlag(isAArea ?  BOOT_FROM_A : BOOT_FROM_B);
}
void Shell_BleConnect(uint32 id1, uint32 id2, uint32 id3, uint32 id4)
{
	Bool bRet = 0;
	int i = 0;
	uint8 phoneId[MAX_PHONE_ID_SIZE];
	
	memcpy(&phoneId[i], &id1, 4);
	i += 4;
	memcpy(&phoneId[i], &id2, 4);
	i += 4;
	memcpy(&phoneId[i], &id3, 4);
	i += 4;
	memcpy(&phoneId[i], &id4, 4);
	i += 4;
	
	if(bRet)
	{
		Printf("PhoneId=");
		DUMP_BYTE(phoneId, i);
	}
	bRet = UtpMgr_PostReq(g_pUtpMgr, REQ_BLE_CONNECT, phoneId, i, 5);
}

void Shell_BleOta()
{
	Arch_PostMsg(g_pMcu, MSG_BLE_OTA_TEST, 0, 0);
}

void Shell_ForceLightingPortToDeviceMode()
{
	g_isResetDetach = False;
	g_pMcu->m_pUdcDev->Reset(g_pMcu->m_pUdcDev);
	g_pMcu->m_pUhcDev->Reset(g_pMcu->m_pUhcDev);
	Mcu_SetUsbSwitch(HUB_DISABLE);
	g_pMcu->m_pUdcDev->Start(g_pMcu->m_pUdcDev);
}

#ifdef CONFIG_QUEUEEX
extern uint8* g_pBulkBuf;
#include "QueueEx.h"
void Shell_GetLogDataEx(int len, uint8 value, Bool isRead)
{
	int i = 0;
	uint8 byte[SECTOR_SIZE];

	memset(byte, value, SECTOR_SIZE);

	QueueEx_PushIn(byte, len);

	if(isRead)
	{
		len = QUEUE_getElementCount((Queue*)g_pQueueEx);
		i = QueueEx_Read((QueueEx*)g_pQueueEx, g_pBulkBuf, len);

		ASRT(i == len);
		
		Printf("Len=%d: \n",i);
		DUMP_BYTE(g_pBulkBuf, i);
	}
}
/*
void Shell_GetLogData(int flag)
{
	if(0 == flag)
	{
		QueueEx_Init(g_pQueueEx);
	}
	if(1 == flag)
	{
		QueueEx_Init(g_pQueueEx);
		Shell_GetLogDataEx(10, 1, True);
		Shell_GetLogDataEx(16, 2, True);
		Shell_GetLogDataEx(510, 3, True);
		Shell_GetLogDataEx(510, 4, True);
		Shell_GetLogDataEx(510, 5, True);
	}
	
	if(2 == flag)
	{
		QueueEx_Init(g_pQueueEx);
		Shell_GetLogDataEx(10, 14, False);
		Shell_GetLogDataEx(16, 15, False);
		Shell_GetLogDataEx(510, 16, False);
		Shell_GetLogDataEx(510, 17, False);
		Shell_GetLogDataEx(510, 18, True);
	}
	
	if(3 == flag)
	{
		QueueEx_Init(g_pQueueEx);
		Shell_GetLogDataEx(512, 21, True);
		Shell_GetLogDataEx(10, 22, True);
		Shell_GetLogDataEx(16, 23, True);
		Shell_GetLogDataEx(510,24, True);
		Shell_GetLogDataEx(511,25, True);
	}
	
	if(4 == flag)
	{
		QueueEx_Init(g_pQueueEx);
		Shell_GetLogDataEx(512, 30, True);
		Shell_GetLogDataEx(10, 31, False);
		Shell_GetLogDataEx(16, 32, False);
		Shell_GetLogDataEx(510, 33, False);
		Shell_GetLogDataEx(488, 34, False);
		Shell_GetLogDataEx(1, 35, True);
	}
}
*/
#endif
void Shell_Init()
{
	static FnDef g_CmdTable[] = 
	{
		  {(void*)Shell_SetDebugBit	, "SetDebugBit(int nIndex, Bool isEnable)"}
		 ,{(void*)Shell_BleConnect		, "BleConnect(uint32 id1; uint32 id2; uint32 id3, uint32 id4, uint32 id4)"}
		 ,{(void*)Debug_Set			, "SetDebugLev(uint8 level/*1=UTP|BLE|OTA;2=USBDEV|USBHOST|USB_DRV...*/)"}

		 ,{(void*)Null, ""}
		 ,{(void*)Shell_Reset		, "Reset()"}
		 ,{(void*)Shell_SetBootFlag	, "SetBootFlag(Bool isAArea)"}
		 ,{(void*)Mcu_SetState		, "SetMcuState(uint8 state/*1=WORK,2=STDANBY,3=IDEL*/)"}
		 ,{(void*)Boot_WriteTestMode, "SetTestMode(Bool isTestModeEnable)"}
		 ,{(void*)Shell_SetPrintfFlag, "SetPrintfFlag(Bool isDefaultPrintf)"}
		 ,{(void*)Shell_SetPmu		, "SetPmu(uint8 pmuState)"}

		 ,{(void*)Null, ""}
		 ,{(void*)Shell_BlePostCmd	, "BleSendCmd(uint8 cmd, int len, uint8 data1, uint8 data2)"}
		 ,{(void*)Shell_GetOemInfo	, "GetOemInfo()"}
		 ,{(void*)Ble_Dump			, "DumpPmu()"}
		 ,{(void*)Iap_Dump			, "DumpIap()"}
		 
		 ,{(void*)Null, ""}
		 ,{(void*)Shell_GetTempPwdEmail	, "GetTempPwdEmail()"}
		 ,{(void*)Shell_WriteSecuInfo	, "SetPwd(const char* pPsw, const char* pEmail, Bool isTempPsw)"}
		 ,{(void*)Shell_SetPhoneID		, "SetPhoneID(const char* pPhoneId)"}
		 ,{(void*)Shell_SetPwdEnalbe	, "EnablePwd(uint8 isPswEnalbe)"}
		 ,{(void*)Shell_VerifyPsw		, "VerifyPwd(uint8_t *pwd)"}
		 ,{(void*)Shell_SetSyncLedEnable, "SetSyncLedEnable(Bool isEnable, Bool isModifiedSdCardByPc)"}
		 ,{(void*)Shell_BleEnable		, "BleEnable(Bool isEnable)"}
		 ,{(void*)Nvd_WritePartNumber	, "SetPartNumber(uint8 cap/*1=16G;2=32G;3=64G;4=128G*/, uint8_t color/*1=WHT,2=BLK,3=RED*/)"}
		 ,{(void*)Shell_NvdErase		, "NvdErase()"}
		 ,{(void*)Nvd_Dump				, "DumpNvd()"}
		 ,{(void*)Shell_BleConnect		, "BleConnect(uint32 id1; uint32 id2; uint32 id3, uint32 id4, uint32 id4)"}
		 ,{(void*)Shell_ForceLightingPortToDeviceMode			, "Force()"}
	};
	
	CmdLine_Init(g_CmdTable, sizeof(g_CmdTable) / sizeof(FnDef), True, (OutPutFun)Printf);

	if(g_OemInfo.m_isDefaultPrintf)
	{
		Printf = __Printf;
		g_dwDebugLevel = g_OemInfo.m_dwDebugLevel;
		//Debug_Set(1);
	}
	else
	{
		//Debug_Set(0);
		g_dwDebugLevel = 0;
		Printf = Printf_None;
	}
}

void Shell_Run()
{
	uint8 buf[8];
	int len = USART_Read(DEBUG_UART_ID, buf, sizeof(buf));
	if(len)
	{
		Shell_String(Null, buf, len);
	}
}

void Shell_String(void* pNull, const uint8* pData, int len)
{
	char input[] = "Printf:?";

	if(g_pMcu->m_State == MCU_START)
	{
		//Mcu_StateWakeup();
	}
	
	if(str_GetFromStream(pData, len, input))
	{
		int size = strlen(input);
		if (input[size-1] == '0')
		{
			//restore_printf_flag(0);
			Shell_SetPrintfFlag(0);
			__Printf("\nPrintf is disable.\n");
			g_dwDebugLevel = 0;
			Printf = Printf_None;
		}
		else
		{
			if(input[size-1] >= '1')
			{
				//restore_printf_flag(1);
				Printf = __Printf;
				//Debug_Set(1);
				Shell_SetPrintfFlag(input[size-1] - '0');
				g_dwDebugLevel = g_OemInfo.m_dwDebugLevel;
				__Printf("\nPrintf is enable, g_dwDebugLevel=0x%08x.\n", g_dwDebugLevel);
			}
		}
		return;
	}
		
	if(len && Printf != Printf_None)
	{
		CmdLine_AddStrEx((char*)pData, len);
	}
}

#endif
