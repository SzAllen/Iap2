#pragma once
#include "TreeNodeData.h"

#if 1
#define MAX_CMD_LEN 64
typedef struct _CmTreeNodeData
{
	UnitTestNodeData m_Base;

	//VerifyFun	Verify;

	UINT8		m_Cmd;		//SensorId
	UINT8		m_NetFn;	//CmValue
	UINT8		m_Len;		//RawValue
	UINT8		m_CmdData[MAX_CMD_LEN];	//Offset, threshold
}CmTreeNodeData;

#endif

class CTester 
{
public:
	CTester(void);
	CTester(TreeNodeData nodeData[], int nCount);
	~CTester(void);
public:
	virtual void OnMsgProc(WPARAM wParam){;};
	static void PrintTreeNodeData(TreeNodeData* pTreeNodeData);
	static void PrintTestResult(TreeNodeData* pTreeNodeData, BOOL isPass);
};

