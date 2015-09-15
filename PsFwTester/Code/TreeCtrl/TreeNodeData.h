#pragma once

struct _TreeNodeData;
typedef Bool (*TesterFun)(struct _TreeNodeData* pNode);
typedef Bool (*TestVerifyFun)(void* pObj, uint32 param[]);
typedef struct _TreeNodeData
{
	int				m_Level;
	LPCTSTR			m_pName;
	TesterFun		Run;
	//TestVerifyFun	Verify;
	//LPCTSTR			m_lpDesc;
	BOOL			m_isExit;
}TreeNodeData;
void TreeNodeData_Dump(TreeNodeData* pTreeNodeData);

typedef struct _UnitTestNodeData
{
	TreeNodeData m_Base;

	uint32		m_Input[10];
	uint32		m_Output[10];
}UnitTestNodeData;

typedef enum _TEST_RESULT
{
	 TEST_PASS = 0
	,TEST_FAILED
	,TEST_PENDING
}TEST_RESULT;

typedef void (*CheckedFun)(void* pObj);
typedef struct _ParamItem
{
	int			m_Level;
	LPCTSTR		m_pName;
	CheckedFun	Checked;

	UINT32		m_Value;
	
	UINT32*		m_pUint32;
	UINT32		m_Mask;

	unsigned int m_isChecked:1;
	unsigned int m_Reseved:30;
	unsigned int m_isAutoFlag:1;
}ParamItem;
