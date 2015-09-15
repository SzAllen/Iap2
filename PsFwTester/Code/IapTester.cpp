#include "StdAfx.h"
#include "IapTester.h"
#include "CaseTreeData.h"

#include "Driver.h"

Bool iAP2_Run(TreeNodeData* pNode)
{
	Iap2CtrlSession g_Iap2CtrlSession;
	Debug_Init();
	Iap2Link_Init(g_pIap2Link, (Iap2LinkTransferFn)Driver_TransferStart);
	Iap2CtrlSession_Init(&g_Iap2CtrlSession);
	//Iap2Link_RegSession(g_pIap2Link, &g_Iap2CtrlSession);
	Iap2Link_Start(g_pIap2Link);
	while(!pNode->m_isExit)
	{
		Driver_Check();
		Iap2Link_Run(g_pIap2Link);
		Sleep(1);
	}
	pNode->m_isExit = FALSE;
	return TRUE;
}

Bool AuthInitData_Txdone(TreeNodeData* pNode)
{
	Driver_TxDone();
	return TRUE;
}

Bool AuthInitData_Rxdone(TreeNodeData* pNode)
{
	static uint8 auth_init_data[] = {0xff,0x55,0x02,0x00,0xee,0x10};
	Driver_RxDone(auth_init_data, sizeof(auth_init_data));
	return TRUE;
}

Bool LinkSync_Rxdone(TreeNodeData* pNode)
{
	static uint8 data[] = {0xFF,0x5A,0x00,0x17,0xC0,0xB0,0x00,0x00,0x20,0x01,0x7F,0xFF,0xFF,0x05,0xDC,0x00,0x0A,0x0A,0x00,0x0A,0x00,0x01,0x82};
	Driver_RxDone(data, sizeof(data));
	return TRUE;
}

Bool Ack_Rxdone(TreeNodeData* pNode)
{
	static uint8 data[] = {0xFF,0x5A,0x00,0x09,0x40,0xB1,0x01,0x00,0xAC};
	Driver_RxDone(data, sizeof(data));
	return TRUE;
}

Bool RequestAuthCerticate_RxDone(TreeNodeData* pNode)
{
	static uint8 data[] = {0xFF,0x5A,0x00,0x10,0x40,0xB1,0x00,0x0A,0x9C,0x40,0x40,0x00,0x06,0xAA,0x00,0xD0 };
	Driver_RxDone(data, sizeof(data));
	return TRUE;
}

Bool RequestChallengeResponse_RxDone(TreeNodeData* pNode)
{
	static uint8 data[] = { 
		 0xFF,0x5A,0x00,0x28,0x40,0xB2,0x01,0x0A,0x82,0x40,0x40,0x00,0x1E,0xAA,0x02,0x00
		,0x18,0x00,0x00,0xEC,0xDB,0x6C,0xAC,0x43,0x86,0x65,0x83,0xD4,0xE9,0x0E,0x07,0xA2
		,0x4C,0x99,0x7E,0xB8,0x1E,0x21,0x44,0xFC};
	Driver_RxDone(data, sizeof(data));
	return TRUE;
}

TreeNodeData g_IapUnitTestItem[] = 
{	
	{1, L"iAP2模块单元测试"}
		,{2, L"iAP2_Run",	(TesterFun)iAP2_Run}
};

TreeNodeData g_IapInputTestItem[] = 
{	
	{1, L"iAP2模块单元测试输入"}
		,{2, L"Tx Done",	(TesterFun)AuthInitData_Txdone}
		,{2, L"AuthInitData Rx Done",	(TesterFun)AuthInitData_Rxdone}
		,{2, L"LinkSync Rx Done",	(TesterFun)LinkSync_Rxdone}
		,{2, L"RequestAuthCerticate Rx Done",	(TesterFun)RequestAuthCerticate_RxDone}
		,{2, L"ACK Rx Done",	(TesterFun)Ack_Rxdone}
		,{2, L"RequestChallengeResponse Rx Done",	(TesterFun)RequestChallengeResponse_RxDone}
};

CIapTester::CIapTester()
{
	GetTreeData()->AddUnitTreeNodeData(g_IapUnitTestItem, sizeof(g_IapUnitTestItem)/sizeof(TreeNodeData));
	GetInputTreeData()->AddUnitTreeNodeData(g_IapInputTestItem, sizeof(g_IapInputTestItem)/sizeof(TreeNodeData));
}

CIapTester g_IapTester;

CIapTester::~CIapTester(void)
{
}
